#include "ServerPlugin.h"
#include "Widgets/CodeWidget.h"

#include <QDebug>
#include <QFileDialog>
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
    : stub(Compiler::NewStub(channel)), mainWindow(mainWindow) {}

void CompilerClient::CompileBuffer(Game* game, CompileMode mode, std::string name) {
  qDebug() << "CompilerBuffer()";
  qDebug() << name.c_str();
  name = "/tmp/holyshit.exe";
  static ClientContext context;
  CompileRequest request;

  request.mutable_game()->CopyFrom(*game);
  request.set_name(name);
  request.set_mode(mode);

  qDebug() << "wtf1";
  emit CompileStatusChanged();
  CompileBufferStream = stub->AsyncCompileBuffer(&context, request, &cq, tag(1));
  qDebug() << "wtf2";
}

void CompilerClient::CompileBuffer(Game* game, CompileMode mode) {
  QTemporaryFile t("enigma");
  if (!t.open()) return;
  CompileBuffer(game, mode, t.fileName().toStdString());
}

void CompilerClient::GetResources() {
  qDebug() << "GetResources()";
  ClientContext context;
  Empty emptyRequest;

  std::unique_ptr<ClientReader<Resource>> reader(stub->GetResources(&context, emptyRequest));
  Resource resource;
  CodeWidget::prepareKeywordStore();
  while (reader->Read(&resource)) {
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
  }
  CodeWidget::finalizeKeywords();
  qDebug() << "done";
  Status status = reader->Finish();
}

void CompilerClient::GetSystems() {
  qDebug() << "GetSystems()";
  ClientContext context;
  Empty emptyRequest;

  std::unique_ptr<ClientReader<SystemType>> reader(stub->GetSystems(&context, emptyRequest));
  SystemType system;
  auto& systemCache = MainWindow::systemCache;
  while (reader->Read(&system)) {
    const QString systemName = QString::fromStdString(system.name());
    qDebug() << systemName;
    systemCache.append(system);
  }

  qDebug() << "done";
  Status status = reader->Finish();
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

void CompilerClient::UpdateLoop() {
  void* got_tag = nullptr;
  bool ok = false;

  auto status = cq.AsyncNext(&got_tag, &ok, future_deadline(0));
  // yield to application main event loop
  if (status != CompletionQueue::NextStatus::GOT_EVENT || !ok || !got_tag) return;

  int msg_id = detag(got_tag);

  static CompileReply reply;

  switch (msg_id) {
    case 1: {
      qDebug() << "ONE";
      CompileBufferStream->Read(&reply, tag(2));
      break;
    }
    default:
      //error
      break;
  }
}

ServerPlugin::ServerPlugin(MainWindow& mainWindow) : RGMPlugin(mainWindow) {
  qDebug() << "heller1";
  process = new QProcess(this);
  qDebug() << "heller2";
  connect(process, &QProcess::readyReadStandardOutput, this, &ServerPlugin::HandleOutput);
  connect(process, &QProcess::readyReadStandardError, this, &ServerPlugin::HandleError);
  process->setWorkingDirectory("../RadialGM/Submodules/enigma-dev");

  qDebug() << "heller3";
  QString program = "emake";
  QStringList arguments;
  arguments << "--server"
            << "-e"
            << "Paths";
  qDebug() << "heller4";
  process->start(program, arguments);
  qDebug() << "heller5";
  process->waitForStarted();
  qDebug() << "heller6";

  // construct the channel and connect to the server
  std::shared_ptr<Channel> channel = CreateChannel("localhost:37818", InsecureChannelCredentials());
  compilerClient = new CompilerClient(channel, mainWindow);
  connect(compilerClient, &CompilerClient::CompileStatusChanged, this, &RGMPlugin::CompileStatusChanged);

  //TODO: timer
  QTimer* timer = new QTimer(this);
  connect(timer, &QTimer::timeout, compilerClient, &CompilerClient::UpdateLoop);
  timer->start();

  // update initial keyword set and systems
  compilerClient->GetResources();
  compilerClient->GetSystems();
}

ServerPlugin::~ServerPlugin() { process->close(); }

void ServerPlugin::Run() { compilerClient->CompileBuffer(mainWindow.Game(), CompileMode::RUN); };

void ServerPlugin::Debug() { compilerClient->CompileBuffer(mainWindow.Game(), CompileMode::DEBUG); };

void ServerPlugin::CreateExecutable() {
  const QString& fileName =
      QFileDialog::getSaveFileName(&mainWindow, tr("Create Executable"), "", tr("Executable (*.exe);;All Files (*)"));
  if (!fileName.isEmpty())
    compilerClient->CompileBuffer(mainWindow.Game(), CompileMode::COMPILE, fileName.toStdString());
};

void ServerPlugin::SetCurrentConfig(const resources::Settings& settings) {
  compilerClient->SetCurrentConfig(settings);
};

void ServerPlugin::HandleOutput() { emit OutputRead(process->readAllStandardOutput()); }

void ServerPlugin::HandleError() { emit ErrorRead(process->readAllStandardError()); }
