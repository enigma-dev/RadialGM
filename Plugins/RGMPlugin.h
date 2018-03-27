#ifndef RGMPLUGIN_H
#define RGMPLUGIN_H

#include "MainWindow.h"

#include <QApplication>

class RGMPlugin : public QObject {
  Q_OBJECT

 public:
  ~RGMPlugin();

 protected:
  const QApplication &app;
  MainWindow &mainWindow;

 protected:
  explicit RGMPlugin(const QApplication &app, MainWindow &mainWindow);
};

#endif  // RGMPLUGIN_H
