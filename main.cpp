#include "main.h"
#include "MainWindow.h"

#include "Dialogs/PreferencesKeys.h"

#include <QApplication>
#include <QSettings>
#include <QStyle>
#include <QStyleFactory>

QString defaultStyle = "";

int main(int argc, char *argv[]) {
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);  //For correct high Dpi Scaling
  QApplication a(argc, argv);
  a.setOrganizationName("ENIGMA Dev Team");
  a.setOrganizationDomain("enigma-dev.org");
  a.setApplicationName("RadialGM");
  a.setWindowIcon(QIcon(":/icon.ico"));
  a.setAttribute(Qt::AA_DisableWindowContextHelpButton);

  defaultStyle = a.style()->objectName();

  QSettings settings;
  settings.beginGroup(preferencesKey());

  settings.beginGroup(appearanceKey());
  const QString &styleName = settings.value(styleNameKey()).toString();
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
