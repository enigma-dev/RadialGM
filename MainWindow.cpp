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
#include "Editors/SettingsEditor.h"
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

QList<buffers::SystemType> MainWindow::systemCache;
MainWindow *MainWindow::m_instance = nullptr;
QScopedPointer<ResourceModelMap> MainWindow::resourceMap;
QScopedPointer<TreeModel> MainWindow::treeModel;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ArtManager::Init();

  m_instance = this;

  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  ui->setupUi(this);
  this->readSettings();
  this->recentFiles = new RecentFiles(this, this->ui->menuRecent, this->ui->actionClearRecentMenu);

  ui->mdiArea->setBackground(QImage(":/banner.png"));
  connect(ui->menuWindow, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

  auto settingsButton = static_cast<QToolButton *>(ui->mainToolBar->widgetForAction(ui->actionSettings));
  settingsButton->setPopupMode(QToolButton::ToolButtonPopupMode::MenuButtonPopup);
  settingsButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
  ui->actionSettings->setMenu(ui->menuChangeGameSettings);

  RGMPlugin *pluginServer = new ServerPlugin(*this);
  auto outputTextBrowser = this->ui->outputTextBrowser;
  connect(pluginServer, &RGMPlugin::LogOutput, outputTextBrowser, &QTextBrowser::append);
  connect(pluginServer, &RGMPlugin::CompileStatusChanged, [=](bool finished) {
    ui->outputDockWidget->show();
    ui->actionRun->setEnabled(finished);
    ui->actionDebug->setEnabled(finished);
    ui->actionCreateExecutable->setEnabled(finished);
  });
  connect(this, &MainWindow::CurrentConfigChanged, pluginServer, &RGMPlugin::SetCurrentConfig);
  connect(ui->actionRun, &QAction::triggered, pluginServer, &RGMPlugin::Run);
  connect(ui->actionDebug, &QAction::triggered, pluginServer, &RGMPlugin::Debug);
  connect(ui->actionCreateExecutable, &QAction::triggered, pluginServer, &RGMPlugin::CreateExecutable);

  openNewProject();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setCurrentConfig(const buffers::resources::Settings &settings) {
  emit m_instance->CurrentConfigChanged(settings);
}

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
                                {TypeCase::kRoom, EditorFactory<RoomEditor>},
                                {TypeCase::kSettings, EditorFactory<SettingsEditor>}});

  auto swIt = subWindows.find(item);
  QMdiSubWindow *subWindow;
  if (swIt == subWindows.end() || !*swIt) {
    auto factoryFunction = factoryMap.find(item->type_case());
    if (factoryFunction == factoryMap.end()) return;  // no registered editor

    ProtoModel *res = resourceMap->GetResourceByName(item->type_case(), item->name());
    BaseEditor *editor = factoryFunction->second(res, this);

    connect(editor, &BaseEditor::ResourceRenamed, resourceMap.get(), &ResourceModelMap::ResourceRenamed);
    connect(editor, &BaseEditor::ResourceRenamed, [=]() { treeModel->dataChanged(QModelIndex(), QModelIndex()); });
    connect(treeModel.get(), &TreeModel::ResourceRenamed, editor,
            [res](TypeCase /*type*/, const QString & /*oldName*/, const QString & /*newName*/) {
              const QModelIndex index = res->index(TreeNode::kNameFieldNumber);
              emit res->dataChanged(index, index);
            });

    subWindow = subWindows[item] = ui->mdiArea->addSubWindow(editor);
    subWindow->resize(subWindow->frameSize().expandedTo(editor->size()));
    editor->setParent(subWindow);

    subWindow->connect(subWindow, &QObject::destroyed, [=]() { subWindows.remove(item); });

    subWindow->setWindowIcon(subWindow->widget()->windowIcon());
    editor->setWindowTitle(QString::fromStdString(item->name()));
  } else {
    subWindow = *swIt;
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

  buffers::Project *loadedProject = nullptr;
  if (suffix == "gm81" || suffix == "gmk" || suffix == "gm6" || suffix == "gmd") {
    loadedProject = gmk::LoadGMK(fName.toStdString());
  } else if (suffix == "gmx") {
    loadedProject = gmx::LoadGMX(fName.toStdString());
  } else if (suffix == "yyp") {
    loadedProject = yyp::LoadYYP(fName.toStdString());
  }

  if (!loadedProject) {
    QMessageBox::warning(this, tr("Failed To Open Project"), tr("There was a problem loading the project: ") + fName,
                         QMessageBox::Ok);
    return;
  }

  MainWindow::setWindowTitle(fileInfo.fileName() + " - ENIGMA");
  recentFiles->prependFile(fName);
  openProject(std::unique_ptr<buffers::Project>(loadedProject));
}

void MainWindow::openNewProject() {
  MainWindow::setWindowTitle(tr("<new game> - ENIGMA"));
  auto newProject = std::unique_ptr<buffers::Project>(new buffers::Project());
  auto *root = newProject->mutable_game()->mutable_root();
  QList<QString> defaultGroups = {tr("Sprites"),   tr("Sounds"),  tr("Backgrounds"), tr("Paths"),
                                  tr("Scripts"),   tr("Shaders"), tr("Fonts"),       tr("Objects"),
                                  tr("Timelines"), tr("Rooms"),   tr("Includes"),    tr("Configs")};
  for (auto groupName : defaultGroups) {
    auto *groupNode = root->add_child();
    groupNode->set_folder(true);
    groupNode->set_name(groupName.toStdString());
  }
  openProject(std::move(newProject));
}

void MainWindow::openProject(std::unique_ptr<buffers::Project> openedProject) {
  this->ui->mdiArea->closeAllSubWindows();
  ArtManager::clearCache();

  project = std::move(openedProject);

  resourceMap.reset(new ResourceModelMap(project->mutable_game()->mutable_root(), nullptr));
  treeModel.reset(new TreeModel(project->mutable_game()->mutable_root(), resourceMap.get(), nullptr));

  ui->treeView->setModel(treeModel.get());
  treeModel->connect(treeModel.get(), &TreeModel::ResourceRenamed, resourceMap.get(),
                     &ResourceModelMap::ResourceRenamed);
}

void MainWindow::on_actionNew_triggered() { openNewProject(); }

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

void MainWindow::CreateResource(TypeCase typeCase) {
  auto *root = this->project->mutable_game()->mutable_root();
  auto child = std::unique_ptr<TreeNode>(new TreeNode());
  auto fieldNum = ResTypeFields[typeCase];
  const Descriptor *desc = child->GetDescriptor();
  const Reflection *refl = child->GetReflection();
  const FieldDescriptor *field = desc->FindFieldByNumber(fieldNum);

  // allocate and set the child's resource field
  refl->MutableMessage(child.get(), field);

  // find a unique name for the new resource
  const QString name = resourceMap->CreateResourceName(child.get());
  child->set_name(name.toStdString());

  this->resourceMap->AddResource(child.get(), resourceMap.get());

  // open the new resource for editing
  openSubWindow(child.get());

  // release ownership of the new child to its parent and the tree
  this->treeModel->addNode(child.release(), root);
}

void MainWindow::on_actionCreate_Sprite_triggered() { CreateResource(TypeCase::kSprite); }

void MainWindow::on_actionCreate_Sound_triggered() { CreateResource(TypeCase::kSound); }

void MainWindow::on_actionCreate_Background_triggered() { CreateResource(TypeCase::kBackground); }

void MainWindow::on_actionCreate_Path_triggered() { CreateResource(TypeCase::kPath); }

void MainWindow::on_actionCreate_Script_triggered() { CreateResource(TypeCase::kScript); }

void MainWindow::on_actionCreate_Shader_triggered() { CreateResource(TypeCase::kShader); }

void MainWindow::on_actionCreate_Font_triggered() { CreateResource(TypeCase::kFont); }

void MainWindow::on_actionCreate_Timeline_triggered() { CreateResource(TypeCase::kTimeline); }

void MainWindow::on_actionCreate_Object_triggered() { CreateResource(TypeCase::kObject); }

void MainWindow::on_actionCreate_Room_triggered() { CreateResource(TypeCase::kRoom); }

void MainWindow::on_actionAddNewConfig_triggered() { CreateResource(TypeCase::kSettings); }

void MainWindow::on_actionRename_triggered() {
  if (!ui->treeView->selectionModel()->hasSelection()) return;
  ui->treeView->edit(ui->treeView->selectionModel()->currentIndex());
}

void MainWindow::on_actionProperties_triggered() {
  if (!ui->treeView->selectionModel()->hasSelection()) return;
  auto selected = ui->treeView->selectionModel()->selectedIndexes();
  for (auto index : selected) {
    auto *treeNode = static_cast<buffers::TreeNode *>(index.internalPointer());
    openSubWindow(treeNode);
  }
}

static void CollectNodes(buffers::TreeNode *root, QSet<buffers::TreeNode *> &cache) {
  cache.insert(root);
  for (int i = 0; i < root->child_size(); ++i) {
    auto *child = root->mutable_child(i);
    cache.insert(child);
    if (child->has_folder()) CollectNodes(child, cache);
  }
}

void MainWindow::on_actionDelete_triggered() {
  if (!ui->treeView->selectionModel()->hasSelection()) return;
  auto selected = ui->treeView->selectionModel()->selectedIndexes();
  QSet<buffers::TreeNode *> selectedNodes;
  for (auto index : selected) {
    auto *treeNode = static_cast<buffers::TreeNode *>(index.internalPointer());
    CollectNodes(treeNode, selectedNodes);
  }
  QString selectedNames = "";
  for (auto node : selectedNodes) {
    selectedNames += (node == *selectedNodes.begin() ? "" : ", ") + QString::fromStdString(node->name());
  }

  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(
      this, tr("Delete Resources"),
      tr("Do you want to delete the following resources from the project?\n%0").arg(selectedNames),
      QMessageBox::Yes | QMessageBox::No);
  if (reply != QMessageBox::Yes) return;

  // close subwindows
  for (auto node : selectedNodes) {
    if (subWindows.contains(node)) subWindows[node]->close();
  }

  // remove tree nodes (recursively unmaps names)
  for (auto index : selected) {
    this->treeModel->removeNode(index);
  }
}

void MainWindow::on_actionExpand_triggered() {
  if (ui->treeView->selectionModel()->hasSelection()) {
    ui->treeView->expand(ui->treeView->selectionModel()->currentIndex());
  } else {
    ui->treeView->expandAll();
  }
}

void MainWindow::on_actionCollapse_triggered() {
  if (ui->treeView->selectionModel()->hasSelection()) {
    ui->treeView->collapse(ui->treeView->selectionModel()->currentIndex());
  } else {
    ui->treeView->collapseAll();
  }
}

void MainWindow::on_actionSortByName_triggered() {
  if (!ui->treeView->selectionModel()->hasSelection()) return;
  treeModel->sortByName(ui->treeView->currentIndex());
}
