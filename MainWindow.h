#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Models/ProtoModel.h"
#include "Models/ResourceModelMap.h"
#include "Models/TreeModel.h"

class MainWindow;
#include "Components/RecentFiles.h"

#include "codegen/project.pb.h"
#include "codegen/server.pb.h"

#include <QList>
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
  static QScopedPointer<ResourceModelMap> resourceMap;
  static QScopedPointer<TreeModel> treeModel;
  static QList<buffers::SystemType> systemCache;

  explicit MainWindow(QWidget *parent);
  ~MainWindow();
  void closeEvent(QCloseEvent *event);
  void openProject(std::unique_ptr<buffers::Project> openedProject);
  buffers::Game *Game() const { return this->project->mutable_game(); }

 signals:
  void CurrentConfigChanged(const buffers::resources::Settings &settings);

 public slots:
  void openFile(QString fName);
  void openNewProject();
  void CreateResource(TypeCase typeCase);
  static void setCurrentConfig(const buffers::resources::Settings &settings);

 private slots:
  // file menu
  void on_actionNew_triggered();
  void on_actionOpen_triggered();
  void on_actionClearRecentMenu_triggered();
  void on_actionPreferences_triggered();
  void on_actionExit_triggered();

  // edit menu
  void on_actionExpand_triggered();
  void on_actionCollapse_triggered();
  void on_actionRename_triggered();
  void on_actionProperties_triggered();
  void on_actionDelete_triggered();
  void on_actionSortByName_triggered();

  // resources menu
  void on_actionCreateSprite_triggered();
  void on_actionCreateSound_triggered();
  void on_actionCreateBackground_triggered();
  void on_actionCreatePath_triggered();
  void on_actionCreateScript_triggered();
  void on_actionCreateShader_triggered();
  void on_actionCreateFont_triggered();
  void on_actionCreateTimeline_triggered();
  void on_actionCreateObject_triggered();
  void on_actionCreateRoom_triggered();
  void on_actionCreateSettings_triggered();

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
  void on_treeView_customContextMenuRequested(const QPoint &pos);

 private:
  static MainWindow *m_instance;

  QHash<buffers::TreeNode *, QMdiSubWindow *> subWindows;

  Ui::MainWindow *ui;

  std::unique_ptr<buffers::Project> project;
  QPointer<RecentFiles> recentFiles;

  void openSubWindow(buffers::TreeNode *item);
  void readSettings();
  void writeSettings();
  void setTabbedMode(bool enabled);
};

#endif  // MAINWINDOW_H
