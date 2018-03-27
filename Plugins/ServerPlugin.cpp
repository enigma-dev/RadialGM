#include "ServerPlugin.h"

#include <QFileDialog>

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
}

ServerPlugin::~ServerPlugin() {
  process->terminate();
  process->waitForFinished();
}

void ServerPlugin::Run(){

};

void ServerPlugin::Debug(){

};

void ServerPlugin::CreateExecutable() {
  const QString& fileName =
      QFileDialog::getSaveFileName(&mainWindow, tr("Create Executable"), "", tr("Executable (*.exe);;All Files (*)"));
  //if (!fileName.isEmpty()) pluginServer->CreateExecutable(fileName);
};

void ServerPlugin::HandleOutput() { emit OutputRead(process->readAllStandardOutput()); }

void ServerPlugin::HandleError() { emit ErrorRead(process->readAllStandardError()); }
