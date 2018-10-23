#ifndef PLUGINSERVER_H
#define PLUGINSERVER_H

#include "RGMPlugin.h"

#define _WIN32_WINNT 0x0600  // at least windows vista required for grpc

#include "codegen/server.grpc.pb.h"

#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc/grpc.h>

#include <QProcess>

using namespace grpc;
using namespace buffers;
using CompileMode = CompileRequest_CompileMode;

class CompilerClient : public QObject {
  Q_OBJECT

 public:
  explicit CompilerClient(std::shared_ptr<Channel> channel, MainWindow& mainWindow);
  void CompileBuffer(Game* game, CompileMode mode, std::string name);
  void CompileBuffer(Game* game, CompileMode mode);
  void GetResources();
  void GetSystems();
  void GetOutput();
  void SetDefinitions(std::string code, std::string yaml);
  void SetCurrentConfig(const resources::Settings& settings);
  void SyntaxCheck();

 signals:
  void CompileStatusChanged(bool finished = false);
  void OutputRead(const QString& output);
  void ErrorRead(const QString& error);

 public slots:
  void UpdateLoop();

 private:
  CompletionQueue cq;
  Status status;

  std::unique_ptr<ClientAsyncReader<CompileReply>> CompileBufferStream;

  std::unique_ptr<Compiler::Stub> stub;
  MainWindow& mainWindow;
};

class ServerPlugin : public RGMPlugin {
  Q_OBJECT

 public:
  ServerPlugin(MainWindow& mainWindow);
  ~ServerPlugin() override;

 public slots:
  void Run() override;
  void Debug() override;
  void CreateExecutable() override;
  void SetCurrentConfig(const buffers::resources::Settings& settings) override;

 private:
  QProcess* process;
  CompilerClient* compilerClient;
};

#endif  // PLUGINSERVER_H
