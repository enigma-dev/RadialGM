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

CompilerClient::CompilerClient(std::shared_ptr<Channel> channel, MainWindow& mainWindow)
    : QObject(&mainWindow), stub(Compiler::NewStub(channel)), mainWindow(mainWindow) {}

void CompilerClient::CompileBuffer(Game* game, CompileMode mode, std::string name) {
  qDebug() << "CompilerBuffer()";
  qDebug() << name.c_str();
  emit CompileStatusChanged();
  auto callData = ScheduleTask<CompileBufferCallData>();
  CompileRequest request;

  request.mutable_game()->CopyFrom(*game);
  request.set_name(name);
  request.set_mode(mode);

  callData->stream = stub->PrepareAsyncCompileBuffer(&callData->context, request, &cq);
  callData->process = [=](const AsyncState state, const Status & /*status*/) -> void {
    auto stream = (ClientAsyncReader<CompileReply>*)callData->stream.get();
    const CompileReply& reply = callData->reply;

    switch (state) {
      case AsyncState::CONNECT: {
        qDebug() << "CONNECT";
        stream->Read(&callData->reply, tag(AsyncState::READ));
        qDebug() << reply.progress().progress() << reply.progress().message().c_str();
        break;
      }
      case AsyncState::READ: {
        qDebug() << "READ";
        qDebug() << reply.progress().progress() << reply.progress().message().c_str();
        for (auto log : reply.message()) {
          emit LogOutput(log.message().c_str());
        }
        stream->Read(&callData->reply, tag(AsyncState::READ));
        break;
      }
      case AsyncState::FINISH: {
        qDebug() << "FINISH";
        emit CompileStatusChanged(true);
        break;
      }
      default:
        break;
    }
  };
}

void CompilerClient::CompileBuffer(Game* game, CompileMode mode) {
  QTemporaryFile* t = new QTemporaryFile(QDir::temp().filePath("enigmaXXXXXX"), &mainWindow);
  if (!t->open()) return;
  t->close();
  CompileBuffer(game, mode, (t->fileName() + ".exe").toStdString());
}

void CompilerClient::GetResources() {
  qDebug() << "GetResources()";
  auto callData = ScheduleTask<GetResourcesCallData>();
  Empty emptyRequest;

  callData->stream = stub->PrepareAsyncGetResources(&callData->context, emptyRequest, &cq);
  callData->process = [=](const AsyncState state, const Status & /*status*/) -> void {
    auto stream = (ClientAsyncReader<Resource>*)callData->stream.get();

    switch (state) {
      case AsyncState::CONNECT: {
        CodeWidget::prepareKeywordStore();
        stream->Read(&callData->resource, tag(AsyncState::READ));
        break;
      }
      case AsyncState::READ: {
        const Resource& resource = callData->resource;
        const QString& name = QString::fromStdString(resource.name().c_str());
        int type = 0;
        if (resource.is_function()) {
          type = 3;
          for (int i = 0; i < resource.overload_count(); ++i) {
            QString overload = QString::fromStdString(resource.parameters(i));
            const QStringRef signature = QStringRef(&overload, overload.indexOf("(") + 1, overload.lastIndexOf(")"));
            CodeWidget::addKeyword(QObject::tr("%0?3(%1)").arg(name).arg(signature));
          }
        } else {
          if (resource.is_global()) type = 1;
          if (resource.is_type_name()) type = 2;
          CodeWidget::addKeyword(name + QObject::tr("?%0").arg(type));
        }
        qDebug() << name << type;
        stream->Read(&callData->resource, tag(AsyncState::READ));
        break;
      }
      case AsyncState::FINISH: {
        CodeWidget::finalizeKeywords();
      }
      default:
        break;
    }
  };
}

void CompilerClient::GetSystems() {
  qDebug() << "GetSystems()";
  auto callData = ScheduleTask<GetSystemsCallData>();
  Empty emptyRequest;

  callData->stream = stub->PrepareAsyncGetSystems(&callData->context, emptyRequest, &cq);
  callData->process = [=](const AsyncState state, const Status & /*status*/) -> void {
    static auto& systemCache = MainWindow::systemCache;
    auto stream = (ClientAsyncReader<SystemType>*)callData->stream.get();
    const SystemType& system = callData->system;
    switch (state) {
      case AsyncState::CONNECT: {
        stream->Read(&callData->system, tag(AsyncState::READ));
        break;
      }
      case AsyncState::READ: {
        const QString systemName = QString::fromStdString(system.name());
        qDebug() << systemName;
        systemCache.append(system);
        stream->Read(&callData->system, tag(AsyncState::READ));
        break;
      }
      default:
        break;
    }
  };
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
  auto& task = this->tasks.front();
  auto& stream = task->stream;
  if (!started) {
    stream->StartCall(tag(AsyncState::CONNECT));
    stream->Finish(&task->status, tag(AsyncState::FINISH));
    started = true;
  }
  auto asyncStatus = cq.AsyncNext(&got_tag, &ok, future_deadline(0));
  // yield to application main event loop
  if (asyncStatus != CompletionQueue::NextStatus::GOT_EVENT || !ok || !got_tag) return;

  AsyncState state = (AsyncState)(detag(got_tag));
  task->process(state, task->status);
  if (state == AsyncState::FINISH) {
    // grpc streams use an arena allocator
    stream.release();
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
