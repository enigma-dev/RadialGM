#include "main.h"
#include "MainWindow.h"

#include <QApplication>
#include <QSettings>
#include <QStyle>
#include <QStyleFactory>

QString defaultStyle = "";

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setOrganizationName("ENIGMA Dev Team");
  a.setOrganizationDomain("enigma-dev.org");
  a.setApplicationName("RadialGM");
  a.setWindowIcon(QIcon(":/icon.ico"));

  defaultStyle = a.style()->objectName();

  QSettings settings;
  settings.beginGroup("Preferences");

  settings.beginGroup("Appearance");
  const QString &styleName = settings.value("styleName").toString();
  QApplication::setStyle(styleName);
  settings.endGroup();  // Preferences/Appearance

  settings.endGroup();  // Preferences

  MainWindow w(nullptr);
  if (argc > 1) {
    w.openFile(QString(argv[1]));
  }
  w.show();

  return a.exec();
}
