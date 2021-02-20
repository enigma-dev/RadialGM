#ifndef RGMPLUGIN_H
#define RGMPLUGIN_H

#include "MainWindow.h"

#include "Settings.pb.h"
#include "server.pb.h"

#include <QApplication>
#include <QTextCharFormat>

class RGMPlugin : public QObject {
  Q_OBJECT

 public:
  ~RGMPlugin();

 signals:
  void LogOutput(const QString &output, const QTextCharFormat &format);
  void CompileStatusChanged(bool finished = false);

 public slots:
  virtual void Run() {}
  virtual void Debug() {}
  virtual void CreateExecutable() {}
  virtual void SetCurrentConfig(const buffers::resources::Settings & /*settings*/) {}

 protected:
  MainWindow &mainWindow;

 protected:
  explicit RGMPlugin(MainWindow &mainWindow);
};

#endif  // RGMPLUGIN_H
