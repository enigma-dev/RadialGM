#include "ServerPlugin.h"
#include "Widgets/CodeWidget.h"

#include <QFileDialog>
#include <QList>
#include <QTemporaryFile>

#include <thread>
#include <memory>

CallData::~CallData() {}

struct LolCookie {
  CallData* callData;
  virtual void fireoff() = 0;
  LolCookie(CallData* callData): callData(callData) {}
};

template <class T>
struct Cookie : public LolCookie {
  void (T::*callback)();
  Cookie(CallData* callData, void (T::*callback)()):
    LolCookie(callData), callback(callback) {}
  virtual void fireoff() {
    (*(T*)callData.*callback)();
  }
};

template <class T>
struct AsyncReadWorker : public CallData {
  T element;
  std::unique_ptr<ClientAsyncReader<T>> stream;

  virtual ~AsyncReadWorker() override {}
  virtual void start() final {
    stream->StartCall(new Cookie<AsyncReadWorker<T>>(this, &started));
  }
  virtual void finish() final {
    stream->Finish(&status, new Cookie<AsyncReadWorker<T>>(this, &finalize));
  }
  virtual void finalize() final {
    finished();
    delete this;
  }

  virtual void started() { finish(); }
  virtual void finished() { }
};

template <class T>
struct AsyncResponseReadWorker : public CallData {
  T element;
  std::unique_ptr<ClientAsyncResponseReader<T>> stream;

  virtual ~AsyncResponseReadWorker() override {}

  virtual void start() final {
    stream->StartCall();
    started();
    //stream->Finish(&element, &status, new Cookie<AsyncResponseReadWorker<T>>(this, &finalize));
  }
  virtual void finish() final {}
  virtual void finalize() final {
    //finished();
    delete this;
  }

  virtual void started() {}
  virtual void finished(const T&) { }
};

struct ResourceReader : public AsyncReadWorker<Resource> {
  virtual ~ResourceReader() {}
  virtual void started() final {
    stream->Read(&element, new Cookie<ResourceReader>(this, &process));
    CodeWidget::prepareKeywordStore();
  }
  virtual void process() final {
    auto resource = element;
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
    stream->Read(&element, new Cookie<ResourceReader>(this, &process));
  }
  virtual void finished() final { CodeWidget::finalizeKeywords(); }
};

struct SystemReader : public AsyncReadWorker<SystemType> {
  virtual ~SystemReader() {}
  virtual void started() final {
    stream->Read(&element, new Cookie<SystemReader>(this, &process));
  }
  virtual void process() final {
    auto system = element;
    static auto& systemCache = MainWindow::systemCache;
    const QString systemName = QString::fromStdString(system.name());
    systemCache.append(system);
    stream->Read(&element, new Cookie<SystemReader>(this, &process));
  }
};

struct CompileReader : public AsyncReadWorker<CompileReply> {
  virtual ~CompileReader() {}
  virtual void started() final {
    stream->Read(&element, new Cookie<CompileReader>(this, &process));
  }
  virtual void process() final {
    auto reply = element;
    for (auto log : reply.message()) {
      emit LogOutput(log.message().c_str());
    }
    stream->Read(&element, new Cookie<CompileReader>(this, &process));
  }
  virtual void finished() final { emit CompileStatusChanged(true); }
};

struct SyntaxCheckReader : public AsyncResponseReadWorker<SyntaxError> {
  virtual ~SyntaxCheckReader() {}
  virtual void finished(const SyntaxError&) final { }
};

CompilerClient::~CompilerClient() {}

CompilerClient::CompilerClient(std::shared_ptr<Channel> channel, MainWindow& mainWindow)
    : QObject(&mainWindow), stub(Compiler::NewStub(channel)), mainWindow(mainWindow) {
  // start a blocking thread to poll for GRPC events
  // and dispatch them back to the GUI thread
  std::thread([this](){
    while(true) {
      void* got_tag = nullptr; bool ok = false;
      // block for next GRPC event, break if shutdown
      if (!this->cq.Next(&got_tag, &ok)) break;
      // block until GUI thread handles GRPC event
      QMetaObject::invokeMethod(this, "UpdateLoop", Qt::BlockingQueuedConnection,
                                Q_ARG(void*, got_tag), Q_ARG(bool, ok));
    }
  }).detach();
}

void CompilerClient::CompileBuffer(Game* game, CompileMode mode, std::string name) {
  emit CompileStatusChanged();

  auto* callData = ScheduleTask<CompileReader>();
  CompileRequest request;

  request.mutable_game()->CopyFrom(*game);
  request.set_name(name);
  request.set_mode(mode);

  auto worker = dynamic_cast<AsyncReadWorker<CompileReply>*>(callData);
  worker->stream = stub->PrepareAsyncCompileBuffer(&worker->context, request, &cq);
  callData->start();
}

void CompilerClient::CompileBuffer(Game* game, CompileMode mode) {
  QTemporaryFile* t = new QTemporaryFile(QDir::temp().filePath("enigmaXXXXXX"), &mainWindow);
  if (!t->open()) return;
  t->close();
  CompileBuffer(game, mode, (t->fileName() + ".exe").toStdString());
}

void CompilerClient::GetResources() {
  auto* callData = ScheduleTask<ResourceReader>();
  Empty emptyRequest;

  auto worker = dynamic_cast<AsyncReadWorker<Resource>*>(callData);
  worker->stream = stub->PrepareAsyncGetResources(&worker->context, emptyRequest, &cq);
  callData->start();
}

void CompilerClient::GetSystems() {
  auto* callData = ScheduleTask<SystemReader>();
  Empty emptyRequest;

  auto worker = dynamic_cast<AsyncReadWorker<SystemType>*>(callData);
  worker->stream = stub->PrepareAsyncGetSystems(&worker->context, emptyRequest, &cq);
  callData->start();
}

void CompilerClient::SetDefinitions(std::string code, std::string yaml) {
  auto* callData = ScheduleTask<SyntaxCheckReader>();
  SetDefinitionsRequest definitionsRequest;

  definitionsRequest.set_code(code);
  definitionsRequest.set_yaml(yaml);

  auto worker = dynamic_cast<AsyncResponseReadWorker<SyntaxError>*>(callData);
  worker->stream = stub->PrepareAsyncSetDefinitions(&worker->context, definitionsRequest, &cq);
  callData->start();
}

void CompilerClient::SetCurrentConfig(const resources::Settings& settings) {
  auto* callData = ScheduleTask<AsyncResponseReadWorker<Empty>>();
  SetCurrentConfigRequest setConfigRequest;
  setConfigRequest.mutable_settings()->CopyFrom(settings);

  auto worker = dynamic_cast<AsyncResponseReadWorker<Empty>*>(callData);
  worker->stream = stub->PrepareAsyncSetCurrentConfig(&worker->context, setConfigRequest, &cq);
  callData->start();
}

void CompilerClient::SyntaxCheck() {
  auto* callData = ScheduleTask<SyntaxCheckReader>();
  SyntaxCheckRequest syntaxCheckRequest;

  auto worker = dynamic_cast<AsyncResponseReadWorker<SyntaxError>*>(callData);
  worker->stream = stub->PrepareAsyncSyntaxCheck(&worker->context, syntaxCheckRequest, &cq);
  callData->start();
}

template <typename T>
T* CompilerClient::ScheduleTask() {
  auto callData = new T();
  connect(callData, &CallData::LogOutput, this, &CompilerClient::LogOutput);
  connect(callData, &CallData::CompileStatusChanged, this, &CompilerClient::CompileStatusChanged);
  return callData;
}

void CompilerClient::UpdateLoop(void* got_tag, bool ok) {
  if (!got_tag) return;
  auto cookie = static_cast<LolCookie*>(got_tag);
  if (!ok) {
    cookie->callData->finish();
    delete cookie;
    return;
  }
  cookie->fireoff();
  delete cookie;
}

ServerPlugin::ServerPlugin(MainWindow& mainWindow) : RGMPlugin(mainWindow) {
  // create a new child process for us to launch an emake server
  process = new QProcess(this);

  // look for an executable file that looks like emake in some common directories
  QList<QString> searchPaths = {QDir::currentPath(), "./enigma-dev", "../RadialGM/Submodules/enigma-dev"};
  QFileInfo emakeFileInfo(QFile("emake"));
  foreach (auto path, searchPaths) {
    const QDir dir(path);
    QDir::Filters filters = QDir::Filter::Executable | QDir::Filter::Files;
    // we use a wildcard because we want it to find emake.exe on Windows
    auto entryList = dir.entryInfoList(QStringList({"emake","emake.exe"}), filters, QDir::SortFlag::NoSort);
    if (!entryList.empty()) {
      emakeFileInfo = entryList.first();
      break;
    }
  }

  // use the closest matching emake file we found and launch it in a child process
  qDebug() << emakeFileInfo.absoluteFilePath();
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
  // Note: gRPC is too dumb to resolve localhost on linux
  std::shared_ptr<Channel> channel = CreateChannel("127.0.0.1:37818", InsecureChannelCredentials());
  compilerClient = new CompilerClient(channel, mainWindow);
  connect(compilerClient, &CompilerClient::CompileStatusChanged, this, &RGMPlugin::CompileStatusChanged);
  // hookup emake's output to our plugin's output signals so it redirects to the
  // main output dock widget (thread safe and don't block the main event loop!)
  connect(compilerClient, &CompilerClient::LogOutput, this, &RGMPlugin::LogOutput);

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
