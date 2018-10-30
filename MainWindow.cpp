#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Dialogs/PreferencesDialog.h"
#include "Dialogs/PreferencesKeys.h"

#include "Editors/BackgroundEditor.h"
#include "Editors/FontEditor.h"
#include "Editors/ObjectEditor.h"
#include "Editors/PathEditor.h"
#include "Editors/RoomEditor.h"
#include "Editors/ScriptEditor.h"
#include "Editors/SoundEditor.h"
#include "Editors/SpriteEditor.h"
#include "Editors/TimelineEditor.h"

#include "Components/ArtManager.h"

#include "Plugins/RGMPlugin.h"
#include "Plugins/ServerPlugin.h"

#include "gmk.h"
#include "gmx.h"
#include "yyp.h"

#include <QtWidgets>

#include <functional>
#include <unordered_map>

#undef GetMessage

ResourceModelMap *MainWindow::resourceMap = nullptr;
TreeModel *MainWindow::treeModel = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ArtManager::Init();

  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  ui->setupUi(this);
  this->readSettings();
  this->recentFiles = new RecentFiles(this, this->ui->menuRecent, this->ui->actionClearRecentMenu);

  ui->mdiArea->setBackground(QImage(":/banner.png"));
  connect(ui->menuWindow, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

  RGMPlugin *pluginServer = new ServerPlugin(*this);
  auto outputTextBrowser = this->ui->outputTextBrowser;
  connect(pluginServer, &RGMPlugin::OutputRead, outputTextBrowser, &QTextBrowser::append);
  connect(pluginServer, &RGMPlugin::ErrorRead, outputTextBrowser, &QTextBrowser::append);
  connect(ui->actionRun, &QAction::triggered, pluginServer, &RGMPlugin::Run);
  connect(ui->actionDebug, &QAction::triggered, pluginServer, &RGMPlugin::Debug);
  connect(ui->actionCreateExecutable, &QAction::triggered, pluginServer, &RGMPlugin::CreateExecutable);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::readSettings() {
  QSettings settings;

  // Restore previous window and dock widget location / state
  settings.beginGroup("MainWindow");
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("state").toByteArray());
  setTabbedMode(settings.value("tabbedView", false).toBool());
  settings.endGroup();
}

void MainWindow::writeSettings() {
  QSettings settings;

  // Save window and dock widget location / state for next session
  settings.beginGroup("MainWindow");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("state", saveState());
  settings.setValue("tabbedView", ui->actionToggleTabbedView->isChecked());
  settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event) {
  ui->mdiArea->closeAllSubWindows();
  this->writeSettings();
  event->accept();
}

template <typename T>
T *EditorFactory(ProtoModel *model, QWidget *parent) {
  return new T(model, parent);
}

void MainWindow::openSubWindow(buffers::TreeNode *item) {
  using namespace google::protobuf;

  using TypeCase = buffers::TreeNode::TypeCase;
  using FactoryMap = std::unordered_map<TypeCase, std::function<BaseEditor *(ProtoModel * m, QWidget * p)>>;

  static FactoryMap factoryMap({{TypeCase::kSprite, EditorFactory<SpriteEditor>},
                                {TypeCase::kSound, EditorFactory<SoundEditor>},
                                {TypeCase::kBackground, EditorFactory<BackgroundEditor>},
                                {TypeCase::kPath, EditorFactory<PathEditor>},
                                {TypeCase::kFont, EditorFactory<FontEditor>},
                                {TypeCase::kScript, EditorFactory<ScriptEditor>},
                                {TypeCase::kTimeline, EditorFactory<TimelineEditor>},
                                {TypeCase::kObject, EditorFactory<ObjectEditor>},
                                {TypeCase::kRoom, EditorFactory<RoomEditor>}});

  auto subWindow = subWindows[item];
  if (!subWindows.contains(item) || subWindow == nullptr) {
    auto factoryFunction = factoryMap.find(item->type_case());
    if (factoryFunction == factoryMap.end()) return;  // no registered editor

    ProtoModel *res = resourceMap->GetResourceByName(item->type_case(), item->name());
    BaseEditor *editor = factoryFunction->second(res, this);

    connect(editor, &BaseEditor::ResourceRenamed, resourceMap, &ResourceModelMap::ResourceRenamed);
    connect(editor, &BaseEditor::ResourceRenamed, [=]() { treeModel->dataChanged(QModelIndex(), QModelIndex()); });

    subWindow = subWindows[item] = ui->mdiArea->addSubWindow(editor);
    subWindow->resize(subWindow->frameSize().expandedTo(editor->size()));
    editor->setParent(subWindow);

    subWindow->connect(subWindow, &QObject::destroyed, [=]() { subWindows.remove(item); });

    subWindow->setWindowIcon(subWindow->widget()->windowIcon());
    subWindow->setWindowTitle(QString::fromStdString(item->name()));
  }

  subWindow->show();
  ui->mdiArea->setActiveSubWindow(subWindow);
}

void MainWindow::updateWindowMenu() {
  static QList<QAction *> windowActions;
  foreach (auto action, windowActions) {
    ui->menuWindow->removeAction(action);
    windowActions.removeOne(action);
  }
  auto windows = ui->mdiArea->subWindowList();
  for (int i = 0; i < windows.size(); ++i) {
    QMdiSubWindow *mdiSubWindow = windows.at(i);

    const auto windowTitle = mdiSubWindow->windowTitle();
    QString numberString = QString::number(i + 1);
    numberString = numberString.insert(numberString.length() - 1, '&');
    QString text = tr("%1 %2").arg(numberString).arg(windowTitle);

    QAction *action = ui->menuWindow->addAction(
        text, mdiSubWindow, [this, mdiSubWindow]() { ui->mdiArea->setActiveSubWindow(mdiSubWindow); });
    windowActions.append(action);
    action->setCheckable(true);
    action->setChecked(mdiSubWindow == ui->mdiArea->activeSubWindow());
  }
}

void MainWindow::openFile(QString fName) {
  QFileInfo fileInfo(fName);
  const QString suffix = fileInfo.suffix();

  if (suffix == "gm81" || suffix == "gmk" || suffix == "gm6" || suffix == "gmd") {
    project = gmk::LoadGMK(fName.toStdString());
  } else if (suffix == "gmx") {
    project = gmx::LoadGMX(fName.toStdString());
  } else if (suffix == "yyp") {
    project = yyp::LoadYYP(fName.toStdString());
  }

  if (!project) {
    QMessageBox::warning(this, tr("Failed To Open Project"), tr("There was a problem loading the project: ") + fName,
                         QMessageBox::Ok);
    return;
  }

  MainWindow::setWindowTitle(fileInfo.fileName() + " - ENIGMA");
  recentFiles->prependFile(fName);

  resourceMap = new ResourceModelMap(project->mutable_game()->mutable_root(), this);
  treeModel = new TreeModel(project->mutable_game()->mutable_root(), resourceMap);
  ui->treeView->setModel(treeModel);
  treeModel->connect(treeModel, &QAbstractItemModel::dataChanged,
                     [=](const QModelIndex &topLeft, const QModelIndex &bottomRight) {
                       for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
                         for (int column = topLeft.column(); column <= bottomRight.column(); ++column) {
                           auto index = topLeft.sibling(row, column);
                           buffers::TreeNode *item = static_cast<buffers::TreeNode *>(index.internalPointer());
                           if (!subWindows.contains(item)) return;
                           auto subWindow = subWindows[item];
                           subWindow->setWindowTitle(QString::fromStdString(item->name()));
                         }
                       }
                     });
  ArtManager::clearCache();
}

void MainWindow::on_actionOpen_triggered() {
  const QString &fileName = QFileDialog::getOpenFileName(
      this, tr("Open Project"), "",
      tr("All supported formats (*.yyp *.project.gmx *.gm81 *.gmk *.gm6 *.gmd);;GameMaker: Studio 2 Projects "
         "(*.yyp);;GameMaker: Studio Projects (*.project.gmx);;Classic "
         "GameMaker Files (*.gm81 *.gmk *.gm6 *.gmd);;All Files (*)"));
  if (!fileName.isEmpty()) openFile(fileName);
}

void MainWindow::on_actionPreferences_triggered() {
  PreferencesDialog preferencesDialog(this);
  preferencesDialog.exec();
}

void MainWindow::on_actionExit_triggered() { QApplication::exit(); }

void MainWindow::setTabbedMode(bool enabled) {
  ui->actionToggleTabbedView->setChecked(enabled);
  ui->mdiArea->setViewMode(enabled ? QMdiArea::TabbedView : QMdiArea::SubWindowView);
  if (enabled) {
    QTabBar *tabBar = ui->mdiArea->findChild<QTabBar *>();
    if (tabBar) {
      tabBar->setExpanding(false);
    }
  }
}

void MainWindow::on_actionCascade_triggered() {
  this->setTabbedMode(false);
  ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_actionTile_triggered() {
  this->setTabbedMode(false);
  ui->mdiArea->tileSubWindows();
}

void MainWindow::on_actionCloseAll_triggered() { ui->mdiArea->closeAllSubWindows(); }

void MainWindow::on_actionCloseOthers_triggered() {
  foreach (QMdiSubWindow *subWindow, ui->mdiArea->subWindowList()) {
    if (subWindow != ui->mdiArea->activeSubWindow()) subWindow->close();
  }
}

void MainWindow::on_actionToggleTabbedView_triggered() { this->setTabbedMode(ui->actionToggleTabbedView->isChecked()); }

void MainWindow::on_actionNext_triggered() { ui->mdiArea->activateNextSubWindow(); }

void MainWindow::on_actionPrevious_triggered() { ui->mdiArea->activatePreviousSubWindow(); }

void MainWindow::on_actionDocumentation_triggered() {
  QUrl url(documentationURL(), QUrl::TolerantMode);
  QDesktopServices::openUrl(url);
}

void MainWindow::on_actionWebsite_triggered() {
  QUrl url(websiteURL(), QUrl::TolerantMode);
  QDesktopServices::openUrl(url);
}

void MainWindow::on_actionCommunity_triggered() {
  QUrl url(communityURL(), QUrl::TolerantMode);
  QDesktopServices::openUrl(url);
}

void MainWindow::on_actionSubmitIssue_triggered() {
  QUrl url(submitIssueURL(), QUrl::TolerantMode);
  QDesktopServices::openUrl(url);
}

void MainWindow::on_actionExploreENIGMA_triggered() { QDesktopServices::openUrl(QUrl(".", QUrl::TolerantMode)); }

void MainWindow::on_actionAbout_triggered() {
  QMessageBox aboutBox(QMessageBox::Information, tr("About"),
                       tr("ENIGMA is a free, open-source, and cross-platform game engine."), QMessageBox::Ok, this,
                       nullptr);
  QAbstractButton *aboutQtButton = aboutBox.addButton(tr("About Qt"), QMessageBox::HelpRole);
  aboutBox.exec();

  if (aboutBox.clickedButton() == aboutQtButton) {
    QMessageBox::aboutQt(this, tr("About Qt"));
  }
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index) {
  buffers::TreeNode *item = static_cast<buffers::TreeNode *>(index.internalPointer());
  const QString name = QString::fromStdString(item->name());

  if (item->has_folder()) {
    return;
  }

  openSubWindow(item);
}

void MainWindow::on_actionClearRecentMenu_triggered() { recentFiles->clear(); }

#include <QDebug>

template <class T>
void MainWindow::CreateResource(TypeCase typeCase) {
  auto *root = this->project->mutable_game()->mutable_root();
  auto *child = root->add_child();
  child->set_name("holyshit");
  auto fieldNum = ResTypeFields[typeCase];
  const Descriptor *desc = child->GetDescriptor();
  const Reflection *refl = child->GetReflection();
  const FieldDescriptor *field = desc->FindFieldByNumber(fieldNum);
  refl->SetAllocatedMessage(child, new T(), field);
  this->resourceMap->AddResource(child, resourceMap);
  this->treeModel->addNode(child, root);
  openSubWindow(child);
}

void MainWindow::on_actionCreate_Sprite_triggered() { CreateResource<buffers::resources::Sprite>(TypeCase::kSprite); }

void MainWindow::on_actionCreate_Sound_triggered() { CreateResource<buffers::resources::Sound>(TypeCase::kSound); }

void MainWindow::on_actionCreate_Background_triggered() {
  CreateResource<buffers::resources::Background>(TypeCase::kBackground);
}

void MainWindow::on_actionCreate_Path_triggered() { CreateResource<buffers::resources::Path>(TypeCase::kPath); }

void MainWindow::on_actionCreate_Script_triggered() { CreateResource<buffers::resources::Script>(TypeCase::kScript); }

void MainWindow::on_actionCreate_Shader_triggered() { CreateResource<buffers::resources::Shader>(TypeCase::kShader); }

void MainWindow::on_actionCreate_Font_triggered() { CreateResource<buffers::resources::Font>(TypeCase::kFont); }

void MainWindow::on_actionCreate_Time_Line_triggered() {
  CreateResource<buffers::resources::Timeline>(TypeCase::kTimeline);
}

void MainWindow::on_actionCreate_Object_triggered() { CreateResource<buffers::resources::Object>(TypeCase::kObject); }

void MainWindow::on_actionCreate_Room_triggered() { CreateResource<buffers::resources::Room>(TypeCase::kRoom); }

void MainWindow::on_actionChange_Global_Game_Settings_triggered() {
  CreateResource<buffers::resources::Settings>(TypeCase::kSettings);
}
