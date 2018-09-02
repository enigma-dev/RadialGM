#ifndef RGMPLUGIN_H
#define RGMPLUGIN_H

#include "MainWindow.h"

#include <QApplication>

class RGMPlugin : public QObject {
  Q_OBJECT

 public:
  ~RGMPlugin();

 signals:
  void OutputRead(const QString &output);
  void ErrorRead(const QString &error);

 public slots:
  virtual void Run() {}
  virtual void Debug() {}
  virtual void CreateExecutable() {}

 protected:
  MainWindow &mainWindow;

 protected:
  explicit RGMPlugin(MainWindow &mainWindow);
};

#endif  // RGMPLUGIN_H
