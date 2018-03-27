#include "ServerPlugin.h"

#define _WIN32_WINNT 0x0600
#include "server.grpc.pb.h"

#include <QFileDialog>
#include <QTemporaryFile>

#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc/grpc.h>

using namespace grpc;
using namespace buffers;

class CompilerClient {
 public:
  explicit CompilerClient(std::shared_ptr<Channel> channel) : stub(Compiler::NewStub(channel)) {}

  void CompileBuffer(const Game& game, std::string name, CompileMode mode) {
    CompileRequest request;
    request.set_name(name);
    request.set_allocated_game(const_cast<Game*>(&game));
    request.set_mode(mode);
    ClientContext context;
    stub->CompileBuffer(&context, request);
  }

  void CompileBuffer(const Game& game, CompileMode mode) {
    QTemporaryFile t(".exe");
    if (!t.open()) return;
    CompileBuffer(game, t.fileName().toStdString(), mode);
  }

 private:
  std::unique_ptr<Compiler::Stub> stub;
};

ServerPlugin::ServerPlugin(MainWindow& mainWindow) : RGMPlugin(mainWindow) {
  process = new QProcess(this);

  connect(process, &QProcess::readyReadStandardOutput, this, &ServerPlugin::HandleOutput);
  connect(process, &QProcess::readyReadStandardError, this, &ServerPlugin::HandleError);
  process->setWorkingDirectory("../RadialGM/Submodules/enigma-dev");

  QString program = "emake";
  QStringList arguments;
  arguments << "--server"
            << "--quiet";
  process->start(program, arguments);

  auto channel = grpc::CreateChannel("localhost:37818", grpc::InsecureChannelCredentials());
  compilerClient = new CompilerClient(channel);
}

ServerPlugin::~ServerPlugin() {
  process->terminate();
  process->waitForFinished();
}

void ServerPlugin::Run() { compilerClient->CompileBuffer(mainWindow.Game(), CompileMode::RUN); };

void ServerPlugin::Debug() { compilerClient->CompileBuffer(mainWindow.Game(), CompileMode::DEBUG); };

void ServerPlugin::CreateExecutable() {
  const QString& fileName =
      QFileDialog::getSaveFileName(&mainWindow, tr("Create Executable"), "", tr("Executable (*.exe);;All Files (*)"));
  if (!fileName.isEmpty())
    compilerClient->CompileBuffer(mainWindow.Game(), fileName.toStdString(), CompileMode::COMPILE);
};

void ServerPlugin::HandleOutput() { emit OutputRead(process->readAllStandardOutput()); }

void ServerPlugin::HandleError() { emit ErrorRead(process->readAllStandardError()); }
