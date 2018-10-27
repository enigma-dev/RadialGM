#ifndef PLUGINSERVER_H
#define PLUGINSERVER_H

#include "RGMPlugin.h"

#define _WIN32_WINNT 0x0600  // at least windows vista required for grpc

#include "codegen/server.grpc.pb.h"

#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/completion_queue.h>
#include <grpc++/create_channel.h>
#include <grpc/grpc.h>

#include <QList>
#include <QPointer>
#include <QProcess>

#include <functional>
#include <memory>
#include <queue>

using namespace grpc;
using namespace buffers;
using CompileMode = CompileRequest_CompileMode;

enum AsyncState { READ = 1, WRITE = 2, CONNECT = 3, WRITES_DONE = 4, FINISH = 5 };

struct CallData {
  Status status;
  ClientContext context;
  std::function<void(const AsyncState, const Status&)> process;
  virtual ~CallData();
  virtual void StartCall(void* tag) = 0;
  virtual void Finish(Status* status, void* tag) = 0;
};

struct CompileBufferCallData : public CallData {
  CompileReply reply;
  std::unique_ptr<ClientAsyncReader<CompileReply>> stream;
  ~CompileBufferCallData() override;
  void StartCall(void* tag) override;
  void Finish(Status* status, void* tag) override;
};

struct GetResourcesCallData : public CallData {
  Resource resource;
  std::unique_ptr<ClientAsyncReader<Resource>> stream;
  ~GetResourcesCallData() override;
  void StartCall(void* tag) override;
  void Finish(Status* status, void* tag) override;
};

struct GetSystemsCallData : public CallData {
  SystemType system;
  std::unique_ptr<ClientAsyncReader<SystemType>> stream;
  ~GetSystemsCallData() override;
  void StartCall(void* tag) override;
  void Finish(Status* status, void* tag) override;
};

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
  void LogOutput(const QString& output);

 public slots:
  void UpdateLoop();

 private:
  CompletionQueue cq;
  std::queue<std::unique_ptr<CallData>> tasks;

  template <typename T>
  T* ScheduleTask();

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
