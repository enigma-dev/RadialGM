#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Models/ProtoModel.h"
#include "Models/TreeModel.h"
#include "Models/DiagnosticModel.h"

class MainWindow;
#include "Components/RecentFiles.h"

#include "project.pb.h"
#include "server.pb.h"

#include <QList>
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QPointer>
#include <QProcess>
#include <QEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

  bool _gameModified; // << don't touch; see property below

 public:
  static QScopedPointer<ProtoModel> protoModel;
  static QScopedPointer<TreeModel> treeModel;
  static QScopedPointer<DiagnosticModel> diagModel;
  static QList<buffers::SystemType> systemCache;

  // this property tracks when the game is edited somewhere
  // it is read only since only the main window sets or clears it
  // when the super model changes and when the game is saved
  // it also marks the main window as modified to inform the user
  // e.g, with an asterisk on Windows or changing the close button on Mac
  // a signal is provided but you should not connect to it willy nilly
  // as it will tell you about all updates to the game even superfluous ones
  Q_PROPERTY(bool gameModified READ IsGameModified WRITE SetGameModified NOTIFY GameWasModified)
  bool IsGameModified() {
    return _gameModified;
  }

  explicit MainWindow(QWidget *parent);
  ~MainWindow();
  void openProject(std::unique_ptr<buffers::Project> openedProject);
  buffers::Game *Game() const { return this->_project->mutable_game(); }

  bool event(QEvent* ev) override {
    if (ev->type() == QEvent::ModifiedChange) {
      // this fixes a bug? in QMdiSubWindow destructor where it
      // indiscriminately clears the ancestor window modified
      // https://bugreports.qt.io/browse/QTBUG-85924
      // for now we workaround by resetting it to our gameModified property
      if (isWindowModified() != IsGameModified())
        setWindowModified(IsGameModified());
    }
    return QWidget::event(ev);
  }

 signals:
  // when the user selects a different config in the main toolbar combo
  void CurrentConfigChanged(const buffers::resources::Settings &settings);
  // sent when the game is edited globally
  // (don't abuse as it can be superfluous most of the time)
  void GameWasModified();

 public slots:
  void openFile(QString fName);
  void openNewProject();
  void openEditor(const QModelIndex& protoIndex);
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
  void on_actionShowDiagnosticInspector_triggered();
  void on_actionAbout_triggered();

  void on_treeView_doubleClicked(const QModelIndex &index);
  void on_treeView_customContextMenuRequested(const QPoint &pos);

  // for only the main window to set or clear the modified
  // status of the game globally after editing and saving
  void SetGameModified(bool mod) {
    _gameModified = mod;
    setWindowModified(mod);
  }

 private:
  void closeEvent(QCloseEvent *event) override;

  static MainWindow *_instance;
  Ui::MainWindow *_ui;

  // super model indexes to open editor windows
  QHash<QPersistentModelIndex, QMdiSubWindow *> _editors;

  std::unique_ptr<buffers::Project> _project;
  QPointer<RecentFiles> _recentFiles;

  void readSettings();
  void writeSettings();
  void setTabbedMode(bool enabled);
};

#endif  // MAINWINDOW_H
