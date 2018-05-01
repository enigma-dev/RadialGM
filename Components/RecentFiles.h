#ifndef RECENTFILES_H
#define RECENTFILES_H

class RecentFiles;
#include "MainWindow.h"

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QPointer>
#include <QString>

class RecentFiles : public QObject {
  Q_OBJECT

 public:
  explicit RecentFiles(QPointer<MainWindow> mainWindow, QPointer<QMenu> menuRecent,
                       QPointer<QAction> actionClearRecentMenu);

  bool empty();
  void prependFile(const QString &fileName);
  void updateActions();
  void clear();

 private:
  QPointer<MainWindow> mainWindow;
  QPointer<QMenu> menuRecent;
  static const int MaxRecentFiles = 10;
  QPointer<QAction> recentFileActs[MaxRecentFiles];

  void recentFileActionTriggered();
};

#endif  // RECENTFILES_H
