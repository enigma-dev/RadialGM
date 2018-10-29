#include "ServerPlugin.h"
#include "Widgets/CodeWidget.h"

#include <QDebug>
#include <QFileDialog>
#include <QList>
#include <QTemporaryFile>
#include <QTimer>

#include <chrono>
#include <memory>

namespace {

inline std::chrono::system_clock::time_point future_deadline(size_t us) {
  return (std::chrono::system_clock::now() + std::chrono::microseconds(us));
}

void* tag(int i) { return reinterpret_cast<void*>(static_cast<intptr_t>(i)); }
int detag(void* p) { return static_cast<int>(reinterpret_cast<intptr_t>(p)); }

}  // anonymous namespace

CallData::~CallData() { qDebug() << "~CallData"; }

template <class T>
struct AsyncReadWorker : public CallData {
  T element;
  std::unique_ptr<ClientAsyncReader<T>> stream;

  virtual ~AsyncReadWorker() override { qDebug() << "~AsyncReadWorker"; }
  void operator()(const AsyncState state, const Status& /*status*/) override {
    switch (state) {
      case AsyncState::CONNECT: {
        started();
        stream->Read(&element, tag(AsyncState::READ));
        break;
      }
      case AsyncState::READ: {
        process(element);
        stream->Read(&element, tag(AsyncState::READ));
        break;
      }
      case AsyncState::FINISH: {
        finished();
        break;
      }
      default:
        // TODO: Report error
        break;
    }
  }
  virtual void start() final { stream->StartCall(tag(AsyncState::CONNECT)); }
  virtual void finish() final { stream->Finish(&status, tag(AsyncState::FINISH)); }

  virtual void started() {}
  virtual void finished() {}
  virtual void process(const T& data) = 0;
};

struct ResourceReader : public AsyncReadWorker<Resource> {
  virtual ~ResourceReader() { qDebug() << "~ResourceReader"; }
  virtual void started() final { CodeWidget::prepareKeywordStore(); }
  virtual void process(const Resource& resource) final {
    const QString& name = QString::fromStdString(resource.name().c_str());
    KeywordType type = KeywordType::UNKNOWN;
    if (resource.is_function()) {
      type = KeywordType::FUNCTION;
      for (int i = 0; i < resource.overload_count(); ++i) {
        QString overload = QString::fromStdString(resource.parameters(i));
        const QStringRef signature = QStringRef(&overload, overload.indexOf("(") + 1, overload.lastIndexOf(")"));
        CodeWidget::addCalltip(name, signature.toString(), type);
      }
    } else {
      if (resource.is_global()) type = KeywordType::GLOBAL;
      if (resource.is_type_name()) type = KeywordType::TYPE_NAME;
      CodeWidget::addKeyword(name, type);
    }
    qDebug() << name << type;
  }
  virtual void finished() final { CodeWidget::finalizeKeywords(); }
};

struct SystemReader : public AsyncReadWorker<SystemType> {
  virtual ~SystemReader() { qDebug() << "~SystemReader"; }
  virtual void process(const SystemType& system) final {
    static auto& systemCache = MainWindow::systemCache;
    const QString systemName = QString::fromStdString(system.name());
    qDebug() << systemName;
    systemCache.append(system);
  }
};

struct CompileReader : public AsyncReadWorker<CompileReply> {
  virtual ~CompileReader() { qDebug() << "~CompileReader"; }
  virtual void process(const CompileReply& reply) final {
    qDebug() << reply.progress().progress() << reply.progress().message().c_str();
    for (auto log : reply.message()) {
      emit LogOutput(log.message().c_str());
    }
  }
  virtual void finished() final { emit CompileStatusChanged(true); }
};

CompilerClient::~CompilerClient() {}

CompilerClient::CompilerClient(std::shared_ptr<Channel> channel, MainWindow& mainWindow)
    : QObject(&mainWindow), stub(Compiler::NewStub(channel)), mainWindow(mainWindow) {}

void CompilerClient::CompileBuffer(Game* game, CompileMode mode, std::string name) {
  qDebug() << "CompilerBuffer()";
  qDebug() << name.c_str();
  emit CompileStatusChanged();

  auto* callData = ScheduleTask<CompileReader>();
  CompileRequest request;

  request.mutable_game()->CopyFrom(*game);
  request.set_name(name);
  request.set_mode(mode);

  auto worker = dynamic_cast<AsyncReadWorker<CompileReply>*>(callData);
  worker->stream = stub->PrepareAsyncCompileBuffer(&worker->context, request, &cq);
}

void CompilerClient::CompileBuffer(Game* game, CompileMode mode) {
  QTemporaryFile* t = new QTemporaryFile(QDir::temp().filePath("enigmaXXXXXX"), &mainWindow);
  if (!t->open()) return;
  t->close();
  CompileBuffer(game, mode, (t->fileName() + ".exe").toStdString());
}

void CompilerClient::GetResources() {
  qDebug() << "GetResources()";
  auto* callData = ScheduleTask<ResourceReader>();
  Empty emptyRequest;

  auto worker = dynamic_cast<AsyncReadWorker<Resource>*>(callData);
  worker->stream = stub->PrepareAsyncGetResources(&worker->context, emptyRequest, &cq);
}

void CompilerClient::GetSystems() {
  qDebug() << "GetSystems()";
  auto* callData = ScheduleTask<SystemReader>();
  Empty emptyRequest;

  auto worker = dynamic_cast<AsyncReadWorker<SystemType>*>(callData);
  worker->stream = stub->PrepareAsyncGetSystems(&worker->context, emptyRequest, &cq);
}

void CompilerClient::SetDefinitions(std::string code, std::string yaml) {
  qDebug() << "SetDefinitions()";
  ClientContext context;
  SetDefinitionsRequest definitionsRequest;

  definitionsRequest.set_code(code);
  definitionsRequest.set_yaml(yaml);

  SyntaxError reply;
  Status status = stub->SetDefinitions(&context, definitionsRequest, &reply);
}

void CompilerClient::SetCurrentConfig(const resources::Settings& settings) {
  qDebug() << "SetCurrentConfig()";
  ClientContext context;
  SetCurrentConfigRequest setConfigRequest;
  setConfigRequest.mutable_settings()->CopyFrom(settings);

  Empty reply;
  Status status = stub->SetCurrentConfig(&context, setConfigRequest, &reply);
}

void CompilerClient::SyntaxCheck() {
  qDebug() << "SyntaxCheck()";
  ClientContext context;
  SyntaxCheckRequest syntaxCheckRequest;

  SyntaxError reply;
  Status status = stub->SyntaxCheck(&context, syntaxCheckRequest, &reply);
}

template <typename T>
T* CompilerClient::ScheduleTask() {
  std::unique_ptr<T> callData(new T());
  tasks.push(std::move(callData));
  return (T*)tasks.back().get();
}

void CompilerClient::UpdateLoop() {
  static bool started = false;
  void* got_tag = nullptr;
  bool ok = false;

  if (this->tasks.empty()) return;
  auto* task = this->tasks.front().get();
  if (!started) {
    connect(task, &CallData::LogOutput, this, &CompilerClient::LogOutput);
    connect(task, &CallData::CompileStatusChanged, this, &CompilerClient::CompileStatusChanged);
    task->start();
    started = true;
  }
  auto asyncStatus = cq.AsyncNext(&got_tag, &ok, future_deadline(0));
  auto state = static_cast<AsyncState>(detag(got_tag));
  if (state != AsyncState::DISCONNECTED && !ok) {
    task->finish();
    return;
  }
  // yield to application main event loop
  if (asyncStatus != CompletionQueue::NextStatus::GOT_EVENT || !got_tag) return;

  (*task)(state, task->status);
  if (state == AsyncState::FINISH) {
    // go to the next task
    tasks.pop();
    // next task needs to be started
    started = false;
  }
}

ServerPlugin::ServerPlugin(MainWindow& mainWindow) : RGMPlugin(mainWindow) {
  // create a new child process for us to launch an emake server
  process = new QProcess(this);

  // look for an executable file that looks like emake in some common directories
  QList<QString> searchPaths = {QDir::currentPath(), "../RadialGM/Submodules/enigma-dev"};
  QFileInfo emakeFileInfo(QFile("emake"));
  foreach (auto path, searchPaths) {
    const QDir dir(path);
    QDir::Filters filters = QDir::Filter::Executable | QDir::Filter::Files;
    // we use a wildcard because we want it to find emake.exe on Windows
    auto entryList = dir.entryInfoList(QStringList("emake*"), filters, QDir::SortFlag::NoSort);
    if (!entryList.empty()) {
      emakeFileInfo = entryList.first();
      break;
    }
  }

  // use the closest matching emake file we found and launch it in a child process
  process->setWorkingDirectory(emakeFileInfo.absolutePath());
  QString program = emakeFileInfo.fileName();
  QStringList arguments;
  arguments << "--server"
            << "-e"
            << "Paths"
            << "-r"
            << "--quiet";

  process->start(program, arguments);
  process->waitForStarted();

  // construct the channel and connect to the server running in the process
  std::shared_ptr<Channel> channel = CreateChannel("localhost:37818", InsecureChannelCredentials());
  compilerClient = new CompilerClient(channel, mainWindow);
  connect(compilerClient, &CompilerClient::CompileStatusChanged, this, &RGMPlugin::CompileStatusChanged);
  // hookup emake's output to our plugin's output signals so it redirects to the
  // main output dock widget (thread safe and don't block the main event loop!)
  connect(compilerClient, &CompilerClient::LogOutput, this, &RGMPlugin::LogOutput);

  // use a timer to process async grpc events at regular intervals
  // without us needing any threading or blocking the main thread
  QTimer* timer = new QTimer(this);
  connect(timer, &QTimer::timeout, compilerClient, &CompilerClient::UpdateLoop);
  timer->start();

  // update initial keyword set and systems
  compilerClient->GetResources();
  compilerClient->GetSystems();
}

ServerPlugin::~ServerPlugin() { process->close(); }

void ServerPlugin::Run() { compilerClient->CompileBuffer(mainWindow.Game(), CompileRequest::RUN); };

void ServerPlugin::Debug() { compilerClient->CompileBuffer(mainWindow.Game(), CompileRequest::DEBUG); };

void ServerPlugin::CreateExecutable() {
  const QString& fileName =
      QFileDialog::getSaveFileName(&mainWindow, tr("Create Executable"), "", tr("Executable (*.exe);;All Files (*)"));
  if (!fileName.isEmpty())
    compilerClient->CompileBuffer(mainWindow.Game(), CompileRequest::COMPILE, fileName.toStdString());
};

void ServerPlugin::SetCurrentConfig(const resources::Settings& settings) {
  compilerClient->SetCurrentConfig(settings);
};
