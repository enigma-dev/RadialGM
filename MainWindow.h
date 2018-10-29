#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Models/ProtoModel.h"
#include "Models/TreeModel.h"
#include "Models/ResourceModelMap.h"

class MainWindow;
#include "Components/RecentFiles.h"

#include "codegen/project.pb.h"

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QPointer>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent);
  ~MainWindow();
  void closeEvent(QCloseEvent *event);
  static ResourceModelMap* resourceMap;
  static TreeModel *treeModel;
  buffers::Game *Game() const { return this->project->mutable_game(); }

 public slots:
  void openFile(QString fName);

 private slots:
  // file menu
  void on_actionOpen_triggered();
  void on_actionClearRecentMenu_triggered();
  void on_actionPreferences_triggered();
  void on_actionExit_triggered();

  // window menu
  void on_actionCascade_triggered();
  void on_actionTile_triggered();
  void on_actionToggleTabbedView_triggered();
  void on_actionCloseAll_triggered();
  void on_actionCloseOthers_triggered();
  void on_actionNext_triggered();
  void on_actionPrevious_triggered();
  void updateWindowMenu();

  // help menu
  void on_actionDocumentation_triggered();
  void on_actionWebsite_triggered();
  void on_actionCommunity_triggered();
  void on_actionSubmitIssue_triggered();
  void on_actionExploreENIGMA_triggered();
  void on_actionAbout_triggered();

  void on_treeView_doubleClicked(const QModelIndex &index);

 private:
  QHash<buffers::TreeNode *, QMdiSubWindow *> subWindows;

  Ui::MainWindow *ui;

  buffers::Project *project;
  QPointer<RecentFiles> recentFiles;

  void openSubWindow(buffers::TreeNode *item);
  void readSettings();
  void writeSettings();
  void setTabbedMode(bool enabled);
};

#endif  // MAINWINDOW_H
