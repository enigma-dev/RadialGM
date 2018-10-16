#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600  // at least windows vista required for grpc
#endif

#include "ServerPlugin.h"
#include "Widgets/CodeWidget.h"

#include "codegen/server.grpc.pb.h"

#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QMenu>
#include <QTemporaryFile>
#include <QWidgetAction>

#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc/grpc.h>

using namespace grpc;
using namespace buffers;

class CompilerClient {
 public:
  explicit CompilerClient(std::shared_ptr<Channel> channel, MainWindow& mainWindow)
      : stub(Compiler::NewStub(channel)), mainWindow(mainWindow) {}

  void CompileBuffer(Game* game, CompileMode mode, std::string name) {
    ClientContext context;
    CompileRequest request;

    request.mutable_game()->CopyFrom(*game);
    request.set_name(name);
    request.set_mode(mode);

    std::unique_ptr<ClientReader<CompileReply> > reader(stub->CompileBuffer(&context, request));
    CompileReply reply;
    while (reader->Read(&reply)) {
      qDebug() << reply.message().c_str() << reply.progress();
    }
    Status status = reader->Finish();
  }

  void CompileBuffer(Game* game, CompileMode mode) {
    QTemporaryFile t("enigma");
    if (!t.open()) return;
    CompileBuffer(game, mode, t.fileName().toStdString());
  }

  void GetResources() {
    qDebug() << "GetResources()";
    ClientContext context;
    Empty emptyRequest;

    std::unique_ptr<ClientReader<Resource> > reader(stub->GetResources(&context, emptyRequest));
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

  void GetSystems() {
    qDebug() << "GetSystems()";
    ClientContext context;
    Empty emptyRequest;

    std::unique_ptr<ClientReader<SystemType> > reader(stub->GetSystems(&context, emptyRequest));
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

  void SetDefinitions(std::string code, std::string yaml) {
    qDebug() << "SetDefinitions()";
    ClientContext context;
    SetDefinitionsRequest definitionsRequest;

    definitionsRequest.set_code(code);
    definitionsRequest.set_yaml(yaml);

    SyntaxError reply;
    Status status = stub->SetDefinitions(&context, definitionsRequest, &reply);
  }

  void SetCurrentConfig(const resources::Settings& settings) {
    qDebug() << "SetCurrentConfig()";
    ClientContext context;
    SetCurrentConfigRequest setConfigRequest;
    setConfigRequest.mutable_settings()->CopyFrom(settings);

    Empty reply;
    Status status = stub->SetCurrentConfig(&context, setConfigRequest, &reply);
  }

  void SyntaxCheck() {
    qDebug() << "SyntaxCheck()";
    ClientContext context;
    SyntaxCheckRequest syntaxCheckRequest;

    SyntaxError reply;
    Status status = stub->SyntaxCheck(&context, syntaxCheckRequest, &reply);
  }

 private:
  std::unique_ptr<Compiler::Stub> stub;
  MainWindow& mainWindow;
};

#include <chrono>
#include <ctime>

ServerPlugin::ServerPlugin(MainWindow& mainWindow) : RGMPlugin(mainWindow) {
  std::shared_ptr<Channel> channel = CreateChannel("localhost:37818", InsecureChannelCredentials());
  ChannelInterface* clangIsAPieceOfShit = (ChannelInterface*)channel.get();

  if (!clangIsAPieceOfShit->WaitForConnected(std::chrono::system_clock::now() + std::chrono::milliseconds(1000))) {
    qDebug() << "heller1";
    process = new QProcess(this);
    qDebug() << "heller2";
    connect(process, &QProcess::readyReadStandardOutput, this, &ServerPlugin::HandleOutput);
    connect(process, &QProcess::readyReadStandardError, this, &ServerPlugin::HandleError);

    qDebug() << "heller3";
    QString program = "emake.exe";
    QStringList arguments;
    arguments << "--server"
              << "--quiet";
    qDebug() << "heller4";
    process->startDetached(program, arguments, "../RadialGM/Submodules/enigma-dev");
    qDebug() << "heller5";
    process->waitForStarted();
    qDebug() << "heller6";
  }
  compilerClient = new CompilerClient(channel, mainWindow);
  compilerClient->GetResources();
  compilerClient->GetSystems();
}

ServerPlugin::~ServerPlugin() {  //process->terminate();
}

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
