#ifndef PLUGINSERVER_H
#define PLUGINSERVER_H

#include "RGMPlugin.h"

#include <QProcess>

class PluginServer : public RGMPlugin {
 public:
  PluginServer(MainWindow &mainWindow);
  ~PluginServer();

 public slots:
  void Run() override;
  void Debug() override;
  void CreateExecutable() override;

 private slots:
  void HandleOutput();
  void HandleError();

 private:
  QProcess *process;
};

#endif  // PLUGINSERVER_H
