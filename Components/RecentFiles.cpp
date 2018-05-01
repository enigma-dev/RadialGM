#include "RecentFiles.h"

#include <QFileInfo>
#include <QSettings>
#include <QString>

RecentFiles::RecentFiles(QPointer<MainWindow> mainWindow, QPointer<QMenu> menuRecent,
                         QPointer<QAction> actionClearRecentMenu)
    : QObject(mainWindow), mainWindow(mainWindow), menuRecent(menuRecent) {
  menuRecent->connect(menuRecent, &QMenu::aboutToShow, this, &RecentFiles::updateActions);
  for (int i = 0; i < MaxRecentFiles; ++i) {
    recentFileActs[i] = menuRecent->addAction(QString(), this, &RecentFiles::recentFileActionTriggered);
    recentFileActs[i]->setVisible(false);
  }

  menuRecent->addSeparator();
  menuRecent->addAction(actionClearRecentMenu);

  menuRecent->setEnabled(this->empty());
}

static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

bool RecentFiles::empty() {
  QSettings settings;

  const int count = settings.beginReadArray(recentFilesKey());
  settings.endArray();
  return count > 0;
}

static QStringList readRecentFiles(QSettings &settings) {
  QStringList result;
  const int count = settings.beginReadArray(recentFilesKey());
  for (int i = 0; i < count; ++i) {
    settings.setArrayIndex(i);
    result.append(settings.value(fileKey()).toString());
  }
  settings.endArray();
  return result;
}

void RecentFiles::updateActions() {
  QSettings settings;

  const QStringList recentFiles = readRecentFiles(settings);
  const int count = qMin(int(MaxRecentFiles), recentFiles.size());
  int i = 0;
  for (; i < count; ++i) {
    const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
    QString numberString = QString::number(i + 1);
    recentFileActs[i]->setText(numberString.insert(numberString.length() - 1, '&') + " " + fileName);
    recentFileActs[i]->setData(recentFiles.at(i));
    recentFileActs[i]->setVisible(true);
  }
  for (; i < MaxRecentFiles; ++i) recentFileActs[i]->setVisible(false);
}

static void writeRecentFiles(const QStringList &files, QSettings &settings) {
  const int count = files.size();
  settings.beginWriteArray(recentFilesKey());
  for (int i = 0; i < count; ++i) {
    settings.setArrayIndex(i);
    settings.setValue(fileKey(), files.at(i));
  }
  settings.endArray();
}

void RecentFiles::prependFile(const QString &fileName) {
  QSettings settings;

  const QStringList oldRecentFiles = readRecentFiles(settings);
  QStringList recentFiles = oldRecentFiles;
  recentFiles.removeAll(fileName);
  recentFiles.prepend(fileName);
  if (oldRecentFiles != recentFiles) writeRecentFiles(recentFiles, settings);

  this->menuRecent->setEnabled(!recentFiles.isEmpty());
}

void RecentFiles::recentFileActionTriggered() {
  if (const QAction *action = qobject_cast<const QAction *>(sender())) mainWindow->openFile(action->data().toString());
}

void RecentFiles::clear() {
  QSettings settings;

  settings.beginWriteArray(recentFilesKey());
  settings.endArray();

  for (int i = 0; i < MaxRecentFiles; ++i) {
    recentFileActs[i]->setVisible(false);
  }

  menuRecent->setEnabled(false);
}
