#ifndef PLUGINSERVER_H
#define PLUGINSERVER_H

#include "RGMPlugin.h"

#include <QProcess>

class CompilerClient;

class ServerPlugin : public RGMPlugin {
 public:
  ServerPlugin(MainWindow &mainWindow);
  ~ServerPlugin() override;

 public slots:
  void Run() override;
  void Debug() override;
  void CreateExecutable() override;
  void SetCurrentConfig(const buffers::resources::Settings &settings) override;

 private slots:
  void HandleOutput();
  void HandleError();

 private:
  QProcess *process;
  CompilerClient *compilerClient;
};

#endif  // PLUGINSERVER_H
