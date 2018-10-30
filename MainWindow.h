#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Models/ProtoModel.h"
#include "Models/ResourceModelMap.h"
#include "Models/TreeModel.h"

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
  static ResourceModelMap *resourceMap;
  static TreeModel *treeModel;
  buffers::Game *Game() const { return this->project->mutable_game(); }

  template <class T>
  void CreateResource(TypeCase typeCase);

 public slots:
  void openFile(QString fName);

 private slots:
  // file menu
  void on_actionOpen_triggered();
  void on_actionClearRecentMenu_triggered();
  void on_actionPreferences_triggered();
  void on_actionExit_triggered();

  // resources menu
  void on_actionCreate_Sprite_triggered();
  void on_actionCreate_Sound_triggered();
  void on_actionCreate_Background_triggered();
  void on_actionCreate_Path_triggered();
  void on_actionCreate_Script_triggered();
  void on_actionCreate_Shader_triggered();
  void on_actionCreate_Font_triggered();
  void on_actionCreate_Time_Line_triggered();
  void on_actionCreate_Object_triggered();
  void on_actionCreate_Room_triggered();
  void on_actionChange_Global_Game_Settings_triggered();

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
