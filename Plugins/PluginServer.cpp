#include "PluginServer.h"

PluginServer::PluginServer(const QApplication &app, MainWindow &mainWindow) : RGMPlugin(app, mainWindow) {
  process = new QProcess(const_cast<QApplication *>(&app));

  connect(process, &QProcess::readyReadStandardOutput, this, &PluginServer::HandleOutput);
  connect(process, &QProcess::readyReadStandardError, this, &PluginServer::HandleError);
  process->setWorkingDirectory("../RadialGM/Submodules/enigma-dev");

  QString program = "emake";
  QStringList arguments;
  arguments << "--server"
            << "--quiet";
  process->start(program, arguments);
}

PluginServer::~PluginServer() {
  process->terminate();
  process->waitForFinished();
}

void PluginServer::HandleOutput() { mainWindow.HandleOutput(process->readAllStandardOutput()); }

void PluginServer::HandleError() { mainWindow.HandleError(process->readAllStandardError()); }
