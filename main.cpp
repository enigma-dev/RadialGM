#include "MainWindow.h"
#include "gmk.h"

#include <QApplication>

#include <iostream>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setOrganizationName("ENIGMA Dev Team");
  a.setOrganizationDomain("enigma-dev.org");
  a.setApplicationName("RadialGM");
  a.setWindowIcon(QIcon(":/icon.ico"));

  int test;
  std::cerr << sizeof(test) << std::endl;
  gmk::bind_output_streams(std::cout, std::cerr);

  MainWindow w(nullptr);
  if (argc > 1) {
    w.openFile(QString(argv[1]));
  }
  w.show();

  return a.exec();
}
