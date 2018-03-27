#include "MainWindow.h"
#include "Plugins/PluginServer.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setWindowIcon(QIcon(":/icon.ico"));
  MainWindow w;
  if (argc > 1) {
    w.openFile(QString(argv[1]));
  }
  w.show();

  PluginServer pluginServer(a, w);

  return a.exec();
}
