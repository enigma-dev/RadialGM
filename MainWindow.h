#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Models/ProtoModel.h"
#include "Models/TreeModel.h"

#include "codegen/project.pb.h"

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  void closeEvent(QCloseEvent *event);
  void openFile(QString fName);

  buffers::Game *Game() const { return this->project->mutable_game(); }

 private slots:
  // file menu
  void on_actionOpen_triggered();
  void on_actionPreferences_triggered();
  void on_actionExit_triggered();

  // window menu
  void on_actionCascade_triggered();
  void on_actionTile_triggered();
  void on_actionToggleTabbedView_triggered();
  void on_actionCloseAll_triggered();
  void on_actionNext_triggered();
  void on_actionPrevious_triggered();

  // help menu
  void on_actionDocumentation_triggered();
  void on_actionWebsite_triggered();
  void on_actionCommunity_triggered();
  void on_actionSubmitIssue_triggered();
  void on_actionExploreENIGMA_triggered();
  void on_actionAbout_triggered();

  void on_treeView_doubleClicked(const QModelIndex &index);

 private:
  QHash<buffers::TreeNode *, ProtoModel *> resourceModels;
  QHash<buffers::TreeNode *, QMdiSubWindow *> subWindows;

  TreeModel *treeModel;
  Ui::MainWindow *ui;

  buffers::Project *project;

  void openSubWindow(buffers::TreeNode *item);
};

#endif  // MAINWINDOW_H
