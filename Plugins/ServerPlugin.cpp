#include "ServerPlugin.h"
#include "MainWindow.h"
#include "Widgets/CodeWidget.h"

#include <QFileDialog>
#include <QList>
#include <QTemporaryFile>
#include <QThread>

#include <thread>
#include <memory>

CallData::~CallData() {}

template <class T>
struct AsyncReadWorker : public CallData {
  T element;
  std::unique_ptr<ClientAsyncReader<T>> stream;

  virtual ~AsyncReadWorker() override {}
  void operator()(const Status& /*status*/) override {
    switch (state) {
      case AsyncState::CONNECT: {
        started();
        state = AsyncState::READ;
        stream->Read(&element, this);
        break;
      }
      case AsyncState::READ: {
        process(element);
        state = AsyncState::READ;
        stream->Read(&element, this);
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
  virtual void start() final {
    state = AsyncState::CONNECT;
    stream->StartCall(this);
  }
  virtual void finish() final {
    state = AsyncState::FINISH;
    stream->Finish(&status, this);
  }

  virtual void started() {}
  virtual void finished() {}
  virtual void process(const T&) = 0;
};

template <class T>
struct AsyncResponseReadWorker : public CallData {
  T element;
  std::unique_ptr<ClientAsyncResponseReader<T>> stream;

  virtual ~AsyncResponseReadWorker() override {}
  void operator()(const Status& /*status*/) override {
    switch (state) {
      case AsyncState::FINISH: {
        finished(element);
        break;
      }
      default:
        // TODO: Report error
        break;
    }
  }
  virtual void start() final {
    stream->StartCall();
    started();
    state = AsyncState::FINISH;
    stream->Finish(&element, &status, this);
  }
  virtual void finish() final {}

  virtual void started() {}
  virtual void finished(const T&) {}
};

struct ResourceReader : public AsyncReadWorker<Resource> {
  virtual ~ResourceReader() {}
  virtual void started() final { CodeWidget::prepareKeywordStore(); }
  virtual void process(const Resource& resource) final {
    const QString& name = QString::fromStdString(resource.name().c_str());
    KeywordType type = KeywordType::UNKNOWN;
    if (resource.is_function()) {
      type = KeywordType::FUNCTION;
      for (int i = 0; i < resource.overload_count(); ++i) {
        QString overload = QString::fromStdString(resource.parameters(i));
        const QString signature = overload.mid(overload.indexOf("(") + 1, overload.lastIndexOf(")"));
        CodeWidget::addCalltip(name, signature, type);
      }
    } else {
      if (resource.is_global()) type = KeywordType::GLOBAL;
      if (resource.is_type_name()) type = KeywordType::TYPE_NAME;
      CodeWidget::addKeyword(name, type);
    }
  }
  virtual void finished() final { CodeWidget::finalizeKeywords(); }
};

struct SystemReader : public AsyncReadWorker<SystemType> {
  virtual ~SystemReader() {}
  virtual void process(const SystemType& system) final {
    static auto& systemCache = MainWindow::systemCache;
    const QString systemName = QString::fromStdString(system.name());
    systemCache.append(system);
  }
};

struct CompileReader : public AsyncReadWorker<CompileReply> {
  virtual ~CompileReader() {}
  virtual void process(const CompileReply& reply) final {
    for (auto log : reply.message()) {
      emit LogOutput(log.message().c_str());
    }
  }
  virtual void finished() final { emit CompileStatusChanged(true); }
};

struct SyntaxCheckReader : public AsyncResponseReadWorker<SyntaxError> {
  virtual ~SyntaxCheckReader() {}
  virtual void finished(const SyntaxError&) final {}
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

void CompilerClient::TearDown() {
  auto* callData = ScheduleTask<AsyncResponseReadWorker<Empty>>();

  auto worker = dynamic_cast<AsyncResponseReadWorker<Empty>*>(callData);
  worker->stream = stub->PrepareAsyncTeardown(&worker->context, ::buffers::Empty(), &cq);
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
  auto callData = static_cast<CallData*>(got_tag);
  if (callData->state != AsyncState::DISCONNECTED && !ok) {
    callData->finish();
    return;
  }

  (*callData)(callData->status);
  if (callData->state == AsyncState::FINISH) {
    delete callData;
  }
}

ServerPlugin::ServerPlugin(MainWindow& mainWindow) : RGMPlugin(mainWindow) {
  // create a new child process for us to launch an emake server
  process = new QProcess(this);

  connect(process, &QProcess::errorOccurred, [&](QProcess::ProcessError error) {
    qDebug() << "QProcess error: " << error << endl;
  });
  connect(process, &QProcess::readyReadStandardOutput, [&]() {
    emit LogOutput(process->readAllStandardOutput());
  });
  connect(process, &QProcess::readyReadStandardError, [&]() {
    emit LogOutput(process->readAllStandardError());
  });

  #ifdef _WIN32
  //TODO: Make all this stuff configurable in IDE
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  // FIXME: this is just an approximate guess on how to get emake running outside a msys shell and currently causes emake to crash
  QString msysPath;
  if (!env.contains("MSYS_ROOT")) {
    msysPath = env.value("SystemDrive", "C:") + "/msys64";
    qDebug().noquote() << "Environmental variable \"MSYS_ROOT\" is not set defaulting MSYS path to: " + msysPath;
  } else msysPath = env.value("MSYS_ROOT");

  env.insert("PATH", env.value("PATH") + ";" + msysPath + "/usr/bin;" + msysPath + "/mingw64/bin");
  process->setProcessEnvironment(env);
  #endif

  // look for an executable file that looks like emake in some common directories
  #ifndef RGM_DEBUG
   QString emakeName = "emake";
  #else
    QString emakeName = "emake-debug";
  #endif

  QFileInfo emakeFileInfo;
  foreach (auto path, MainWindow::EnigmaSearchPaths) {
    const QDir dir(path);
    QDir::Filters filters = QDir::Filter::Executable | QDir::Filter::Files;
    auto entryList = dir.entryInfoList(QStringList({emakeName, emakeName + ".exe"}), filters, QDir::SortFlag::NoSort);
    if (!entryList.empty()) {
      emakeFileInfo = entryList.first();
      break;
    }
  }

  if (emakeFileInfo.filePath().isEmpty()) {
    qDebug() << "Error: Failed to locate emake. Compiling and syntax check will not work.\n" << "Search Paths:\n" << MainWindow::EnigmaSearchPaths;
    return;
  }

  if (MainWindow::EnigmaRoot.filePath().isEmpty()) {
    qDebug() << "Error: Failed to locate ENIGMA sources. Compiling and syntax check will not work.\n" << "Search Paths:\n" << MainWindow::EnigmaSearchPaths;
    return;
  }

  // use the closest matching emake file we found and launch it in a child process
  qDebug() << "Using emake exe at: " << emakeFileInfo.absolutePath();
  qDebug() << "Using ENIGMA sources at: " << MainWindow::EnigmaRoot.absolutePath();
  process->setWorkingDirectory(emakeFileInfo.absolutePath());
  QString program = emakeFileInfo.fileName();
  QStringList arguments;
  arguments << "--server"
            << "-e"
            << "Paths"
            << "-r"
            << "--quiet"
            << "--enigma-root"
            << MainWindow::EnigmaRoot.absolutePath();

  qDebug() << "Running: " << program << " " << arguments;

  process->start(program, arguments);
  process->waitForStarted();

  QThread::sleep(5);

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

ServerPlugin::~ServerPlugin() {
  compilerClient->TearDown();
  process->waitForFinished();
}

void ServerPlugin::Run() { compilerClient->CompileBuffer(mainWindow.Game(), CompileRequest::RUN); }

void ServerPlugin::Debug() { compilerClient->CompileBuffer(mainWindow.Game(), CompileRequest::DEBUG); }

void ServerPlugin::CreateExecutable() {
  const QString& fileName =
      QFileDialog::getSaveFileName(&mainWindow, tr("Create Executable"), "", tr("Executable (*.exe);;All Files (*)"));
  if (!fileName.isEmpty())
    compilerClient->CompileBuffer(mainWindow.Game(), CompileRequest::COMPILE, fileName.toStdString());
};

void ServerPlugin::SetCurrentConfig(const resources::Settings& settings) {
  compilerClient->SetCurrentConfig(settings);
};
