#ifndef PLUGINSERVER_H
#define PLUGINSERVER_H

#include "RGMPlugin.h"

#include <QProcess>

class PluginServer : public RGMPlugin {
 public:
  PluginServer(const QApplication &app, MainWindow &mainWindow);
  ~PluginServer();

 private slots:
  void HandleOutput();
  void HandleError();

 private:
  QProcess *process;
};

#endif  // PLUGINSERVER_H
