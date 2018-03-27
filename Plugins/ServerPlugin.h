#ifndef PLUGINSERVER_H
#define PLUGINSERVER_H

#include "RGMPlugin.h"

#include <QProcess>

class ServerPlugin : public RGMPlugin {
 public:
  ServerPlugin(MainWindow &mainWindow);
  ~ServerPlugin() override;

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
