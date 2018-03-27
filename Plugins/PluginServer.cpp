#include "PluginServer.h"

#include <QFileDialog>

PluginServer::PluginServer(MainWindow& mainWindow) : RGMPlugin(mainWindow) {
  process = new QProcess(this);

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

void PluginServer::Run(){

};

void PluginServer::Debug(){

};

void PluginServer::CreateExecutable() {
  const QString& fileName =
      QFileDialog::getSaveFileName(&mainWindow, tr("Create Executable"), "", tr("Executable (*.exe);;All Files (*)"));
  //if (!fileName.isEmpty()) pluginServer->CreateExecutable(fileName);
};

void PluginServer::HandleOutput() { emit OutputRead(process->readAllStandardOutput()); }

void PluginServer::HandleError() { emit ErrorRead(process->readAllStandardError()); }
