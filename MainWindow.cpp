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
#include "Editors/ShaderEditor.h"
#include "Editors/SoundEditor.h"
#include "Editors/SpriteEditor.h"
#include "Editors/TimelineEditor.h"

#include "Components/ArtManager.h"
#include "Components/Logger.h"

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

static QTextEdit *diagnosticTextEdit = nullptr;
static QAction *toggleDiagnosticsAction = nullptr;

void diagnosticHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  if (toggleDiagnosticsAction && !toggleDiagnosticsAction->isChecked()) {
    toggleDiagnosticsAction->setIcon(QIcon(":/actions/log-debug.png"));
    toggleDiagnosticsAction->setToolTip(
        QT_TR_NOOP("The editor encountered issues which have been logged in the diagnostics output."));
  } else {
    // this should never happen!
    // NOTE: if we used R_EXPECT_V here it would be recursive
    std::cerr << "Critical: Diagnostics toggle button does not exist!" << std::endl;
  }

  QByteArray localMsg = msg.toLocal8Bit();
  QString file = context.file ? context.file : "";
  QString function = context.function ? context.function : "";
  QString msgFormat("%1 in %3:%4 aka %5:\n\t%2");
  QString typeName = "Unknown:";
  switch (type) {
    case QtDebugMsg: typeName = "Debug"; break;
    case QtInfoMsg: typeName = "Info"; break;
    case QtWarningMsg: typeName = "Warning"; break;
    case QtCriticalMsg: typeName = "Critical"; break;
    case QtFatalMsg: typeName = "Fatal"; break;
  }
  QString msgFormatted = msgFormat.arg(typeName, localMsg.constData(), file, QString::number(context.line), function);

  std::cerr << msgFormatted.toStdString() << std::endl;

  if (diagnosticTextEdit) {
    diagnosticTextEdit->append(msgFormatted);
  } else {
    // this should never happen!
    std::cerr << "Critical: Diagnostics text control does not exist!" << std::endl;
  }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ArtManager::Init();

  m_instance = this;

  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  ui->setupUi(this);

  QToolBar *outputTB = new QToolBar(this);
  outputTB->setIconSize(QSize(24, 24));
  toggleDiagnosticsAction = new QAction();
  toggleDiagnosticsAction->setCheckable(true);
  toggleDiagnosticsAction->setIcon(QIcon(":/actions/log.png"));
  toggleDiagnosticsAction->setToolTip(tr("Toggle Editor Diagnostics"));
  outputTB->addAction(toggleDiagnosticsAction);
  outputTB->addSeparator();
  // use tool button for clear because QAction always has tooltip
  QToolButton *clearButton = new QToolButton();
  clearButton->setText(tr("Clear"));
  outputTB->addWidget(clearButton);
  QVBoxLayout *outputLayout = static_cast<QVBoxLayout *>(ui->outputDockWidgetContents->layout());
  outputLayout->insertWidget(0, outputTB);

  // install editor diagnostics handler
  diagnosticTextEdit = ui->debugTextBrowser;
  qInstallMessageHandler(diagnosticHandler);

  connect(clearButton, &QToolButton::clicked,
          [=]() { (toggleDiagnosticsAction->isChecked() ? ui->debugTextBrowser : ui->outputTextBrowser)->clear(); });
  connect(toggleDiagnosticsAction, &QAction::toggled, [=](bool checked) {
    ui->outputStackedWidget->setCurrentIndex(checked);

    // reset the log icon as soon as diagnostics is viewed
    if (checked) {
      toggleDiagnosticsAction->setIcon(QIcon(":/actions/log.png"));
      toggleDiagnosticsAction->setToolTip(tr("Toggle Editor Diagnostics"));
    }
  });

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

  connect(ui->mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::MDIWindowChanged);

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
T *EditorFactory(ProtoModelPtr model, QWidget *parent) {
  return new T(model, parent);
}

void MainWindow::openSubWindow(buffers::TreeNode *item) {
  using namespace google::protobuf;

  using TypeCase = buffers::TreeNode::TypeCase;
  using FactoryMap = std::unordered_map<TypeCase, std::function<BaseEditor *(ProtoModelPtr m, QWidget * p)>>;

  static FactoryMap factoryMap({{TypeCase::kSprite, EditorFactory<SpriteEditor>},
                                {TypeCase::kSound, EditorFactory<SoundEditor>},
                                {TypeCase::kBackground, EditorFactory<BackgroundEditor>},
                                {TypeCase::kPath, EditorFactory<PathEditor>},
                                {TypeCase::kFont, EditorFactory<FontEditor>},
                                {TypeCase::kScript, EditorFactory<ScriptEditor>},
                                {TypeCase::kShader, EditorFactory<ShaderEditor>},
                                {TypeCase::kTimeline, EditorFactory<TimelineEditor>},
                                {TypeCase::kObject, EditorFactory<ObjectEditor>},
                                {TypeCase::kRoom, EditorFactory<RoomEditor>},
                                {TypeCase::kSettings, EditorFactory<SettingsEditor>}});

  auto swIt = subWindows.find(item);
  QMdiSubWindow *subWindow;
  if (swIt == subWindows.end() || !*swIt) {
    auto factoryFunction = factoryMap.find(item->type_case());
    if (factoryFunction == factoryMap.end()) return;  // no registered editor

    ProtoModelPtr res = resourceMap->GetResourceByName(item->type_case(), item->name());
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

void MainWindow::MDIWindowChanged(QMdiSubWindow *window) {
  for (QMdiSubWindow *subWindow : subWindows) {
    if (subWindow == nullptr) continue;
    BaseEditor *editor = static_cast<BaseEditor *>(subWindow->widget());
    if (window == subWindow) {
      emit editor->FocusGained();
    } else if (editor->HasFocus()) {
      emit editor->FocusLost();
    }
  }
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
  QList<QString> defaultGroups = {tr("Sprites"), tr("Sounds"),  tr("Backgrounds"), tr("Paths"),
                                  tr("Scripts"), tr("Shaders"), tr("Fonts"),       tr("Timelines"),
                                  tr("Objects"), tr("Rooms"),   tr("Includes"),    tr("Configs")};
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
  // open the new resource for editing
  this->resourceMap->AddResource(child.get());
  openSubWindow(child.get());
  // release ownership of the new child to its parent and the tree
  auto index = this->treeModel->addNode(child.release(), ui->treeView->currentIndex());

  // select the new node so that it gets "revealed" and its parent is expanded
  ui->treeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  // start editing the name of the resource in the tree for convenience
  ui->treeView->edit(index);
}

void MainWindow::on_actionCreateSprite_triggered() { CreateResource(TypeCase::kSprite); }

void MainWindow::on_actionCreateSound_triggered() { CreateResource(TypeCase::kSound); }

void MainWindow::on_actionCreateBackground_triggered() { CreateResource(TypeCase::kBackground); }

void MainWindow::on_actionCreatePath_triggered() { CreateResource(TypeCase::kPath); }

void MainWindow::on_actionCreateScript_triggered() { CreateResource(TypeCase::kScript); }

void MainWindow::on_actionCreateShader_triggered() { CreateResource(TypeCase::kShader); }

void MainWindow::on_actionCreateFont_triggered() { CreateResource(TypeCase::kFont); }

void MainWindow::on_actionCreateTimeline_triggered() { CreateResource(TypeCase::kTimeline); }

void MainWindow::on_actionCreateObject_triggered() { CreateResource(TypeCase::kObject); }

void MainWindow::on_actionCreateRoom_triggered() { CreateResource(TypeCase::kRoom); }

void MainWindow::on_actionCreateSettings_triggered() { CreateResource(TypeCase::kSettings); }

void MainWindow::on_actionDuplicate_triggered() {
  if (!ui->treeView->selectionModel()->hasSelection()) return;
  const auto index = ui->treeView->selectionModel()->currentIndex();
  const auto *node = static_cast<const buffers::TreeNode *>(index.internalPointer());
  if (node->has_folder()) return;

  // duplicate the node
  auto *dup = treeModel->duplicateNode(*node);
  // insert the duplicate into the tree
  const auto dupIndex = treeModel->insert(index.parent(), index.row() + 1, dup);
  // open an editor for the duplicate node
  openSubWindow(dup);

  // select the new node so that it gets "revealed" and its parent is expanded
  ui->treeView->selectionModel()->setCurrentIndex(dupIndex, QItemSelectionModel::ClearAndSelect);
  // start editing the name of the resource in the tree for convenience
  ui->treeView->edit(dupIndex);
}

void MainWindow::on_actionCreateGroup_triggered() {
  auto child = std::unique_ptr<TreeNode>(new TreeNode());
  child->set_folder(true);

  // find a unique name for the new group
  const QString name = resourceMap->CreateResourceName(TypeCase::kFolder, "group");
  child->set_name(name.toStdString());
  // release ownership of the new child to its parent and the tree
  this->resourceMap->AddResource(child.get());
  auto index = this->treeModel->addNode(child.release(), ui->treeView->currentIndex());

  // select the new node so that it gets "revealed" and its parent is expanded
  ui->treeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  // start editing the name of the resource in the tree for convenience
  ui->treeView->edit(index);
}

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

void MainWindow::on_actionExpand_triggered() { ui->treeView->expandAll(); }

void MainWindow::on_actionCollapse_triggered() { ui->treeView->collapseAll(); }

void MainWindow::on_actionSortByName_triggered() {
  if (!ui->treeView->selectionModel()->hasSelection()) return;
  treeModel->sortByName(ui->treeView->currentIndex());
}

void MainWindow::on_treeView_customContextMenuRequested(const QPoint &pos) {
  ui->menuEdit->exec(ui->treeView->mapToGlobal(pos));
}
