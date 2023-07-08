#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Models/ProtoModel.h"
#include "Models/ResourceModelMap.h"
#include "Models/TreeModel.h"
#include "Editors/BaseEditor.h"

class MainWindow;
#include "Components/RecentFiles.h"

#include "project.pb.h"
#include "server.pb.h"
#include "event_reader/event_parser.h"
#include "egm.h"

#include <QList>
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QPointer>
#include <QProcess>
#include <QFileInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  static ResourceModelMap* resourceMap;
  static MessageModel* resourceModel;
  static TreeModel* treeModel;
  static QList<buffers::SystemType> systemCache;

  explicit MainWindow(QWidget *parent);
  ~MainWindow();
  void openProject(std::unique_ptr<buffers::Project> openedProject);
  buffers::Game *Game() const { return this->_project->mutable_game(); }

  static QList<QString> EnigmaSearchPaths;
  static QFileInfo EnigmaRoot;
  static EventData* GetEventData() { return _event_data.get(); }

  typedef BaseEditor *EditorFactoryFunction(MessageModel *model, MainWindow *parent);
  void openSubWindow(MessageModel *res, EditorFactoryFunction factory_function);

 signals:
  void CurrentConfigChanged(const buffers::resources::Settings &settings);

 public slots:
  void openFile(QString fName);
  void openNewProject();
  void CreateResource(TypeCase typeCase);
  void ResourceModelDeleted(MessageModel* m);
  static void setCurrentConfig(const buffers::resources::Settings &settings);

 private slots:
  // file menu
  void on_actionNew_triggered();
  void on_actionOpen_triggered();
  void on_actionClearRecentMenu_triggered();
  void on_actionSave_triggered();
  void on_actionPreferences_triggered();
  void on_actionExit_triggered();

  // edit menu
  void on_actionDuplicate_triggered();
  void on_actionCreateGroup_triggered();
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
  void closeEvent(QCloseEvent *event) override;

  static MainWindow *_instance;

  QHash<const MessageModel *, QMdiSubWindow *> _subWindows;

  Ui::MainWindow *_ui;

  std::unique_ptr<buffers::Project> _project;
  QPointer<RecentFiles> _recentFiles;

  static std::unique_ptr<EventData> _event_data;

  void readSettings();
  void writeSettings();
  void setTabbedMode(bool enabled);
  static QFileInfo getEnigmaRoot();
};

#endif  // MAINWINDOW_H
