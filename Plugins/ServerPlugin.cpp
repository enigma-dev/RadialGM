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

    QWidget* widget = new QWidget();
    QWidgetAction* widgetAction = new QWidgetAction(&mainWindow);
    QFormLayout* layout = new QFormLayout();
    layout->setSpacing(4);
    layout->setMargin(4);
    layout->setLabelAlignment(Qt::AlignmentFlag::AlignRight);
    widget->setLayout(layout);
    widgetAction->setDefaultWidget(widget);

    QMenu* extensionsMenu = new QMenu();

    std::unique_ptr<ClientReader<SystemType> > reader(stub->GetSystems(&context, emptyRequest));
    SystemType system;
    while (reader->Read(&system)) {
      const QString systemName = QString::fromStdString(system.name());
      qDebug() << systemName;

      if (systemName.toLower() == "extensions") {
        for (auto extension : system.subsystems()) {
          const QString extensionName = QString::fromStdString(extension.name());
          QAction* extensionAction = extensionsMenu->addAction(extensionName);
          extensionAction->setCheckable(true);
        }
        continue;
      }

      QLabel* label = new QLabel(systemName);
      QComboBox* combo = new QComboBox();
      layout->addRow(label, combo);

      for (auto subsystem : system.subsystems()) {
        const QString subsystemName = QString::fromStdString(subsystem.name());
        combo->addItem(subsystemName);
        //qDebug() << subsystem.name().c_str();
        //qDebug() << subsystem.id().c_str();
        //qDebug() << subsystem.description().c_str();
        //qDebug() << subsystem.target().c_str();
      }
    }

    QAction* extensionsSeperator = new QAction();
    extensionsSeperator->setSeparator(true);
    QAction* extensionsMenuAction = new QAction(QObject::tr("Extensions"));
    extensionsMenuAction->setMenu(extensionsMenu);

    mainWindow.addSystemMenu(extensionsMenuAction);
    mainWindow.addSystemMenu(extensionsSeperator);
    mainWindow.addSystemMenu(widgetAction);

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

void ServerPlugin::HandleOutput() { emit OutputRead(process->readAllStandardOutput()); }

void ServerPlugin::HandleError() { emit ErrorRead(process->readAllStandardError()); }
