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

#ifdef RGM_SERVER_ENABLED
#include "Plugins/ServerPlugin.h"
#endif

#include "gmk.h"
#include "gmx.h"
#include "yyp.h"

#include <QtWidgets>

#include <functional>
#include <unordered_map>
#include <QFile>
#include <sstream>

#undef GetMessage

QList<QString> MainWindow::EnigmaSearchPaths = {
  QDir::currentPath(), "./enigma-dev", "../enigma-dev", "../RadialGM/Submodules/enigma-dev"
};
QFileInfo MainWindow::EnigmaRoot = MainWindow::getEnigmaRoot();
QList<buffers::SystemType> MainWindow::systemCache;
MainWindow *MainWindow::_instance = nullptr;
QScopedPointer<ResourceModelMap> MainWindow::resourceMap;
QScopedPointer<TreeModel> MainWindow::treeModel;
std::unique_ptr<EventData> MainWindow::_event_data;

static QTextEdit *diagnosticTextEdit = nullptr;
static QAction *toggleDiagnosticsAction = nullptr;

void diagnosticHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  if (toggleDiagnosticsAction) {
    if (!toggleDiagnosticsAction->isChecked()) {
      toggleDiagnosticsAction->setIcon(QIcon(":/actions/log-debug.png"));
      toggleDiagnosticsAction->setToolTip(
          QT_TR_NOOP("The editor encountered issues which have been logged in the diagnostics output."));
    }
  } else {
    // this should never happen!
    // NOTE: if we used R_EXPECT_V here it would be infinite recursion
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

  if (diagnosticTextEdit != nullptr) {
    diagnosticTextEdit->append(msgFormatted);
  } else {
    // this should never happen!
    std::cerr << "Critical: Diagnostics text control does not exist!" << std::endl;
  }
}

QFileInfo MainWindow::getEnigmaRoot() {
  QFileInfo EnigmaRoot;
  foreach (auto path, EnigmaSearchPaths) {
    const QDir dir(path);
    QDir::Filters filters = QDir::Filter::AllEntries;
    auto entryList = dir.entryInfoList(QStringList({"ENIGMAsystem"}), filters, QDir::SortFlag::NoSort);
    if (!entryList.empty()) {
      EnigmaRoot = entryList.first();
        break;
    }
  }

  return EnigmaRoot;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow){

  if (!EnigmaRoot.filePath().isEmpty()) {
    _event_data = std::make_unique<EventData>(ParseEventFile((EnigmaRoot.absolutePath() + "/events.ey").toStdString()));
  } else {
    qDebug() << "Error: Failed to locate ENIGMA sources. Loading internal events.ey.\n" << "Search Paths:\n"
             << MainWindow::EnigmaSearchPaths;
    QFile internal_events(":/events.ey");
    internal_events.open(QIODevice::ReadOnly | QFile::Text);
    std::stringstream ss;
    ss << internal_events.readAll().toStdString();
    _event_data = std::make_unique<EventData>(ParseEventFile(ss));
  }

  egm::LibEGMInit(_event_data.get());

  ArtManager::Init();

  _instance = this;

  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  _ui->setupUi(this);

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
  QVBoxLayout *outputLayout = static_cast<QVBoxLayout *>(_ui->outputDockWidgetContents->layout());
  outputLayout->insertWidget(0, outputTB);

  // install editor diagnostics handler
  diagnosticTextEdit = _ui->debugTextBrowser;
  qInstallMessageHandler(diagnosticHandler);

  connect(clearButton, &QToolButton::clicked,
          [=]() { (toggleDiagnosticsAction->isChecked() ? _ui->debugTextBrowser : _ui->outputTextBrowser)->clear(); });
  connect(toggleDiagnosticsAction, &QAction::toggled, [=](bool checked) {
    _ui->outputStackedWidget->setCurrentIndex(checked);

    // reset the log icon as soon as diagnostics is viewed
    if (checked) {
      toggleDiagnosticsAction->setIcon(QIcon(":/actions/log.png"));
      toggleDiagnosticsAction->setToolTip(tr("Toggle Editor Diagnostics"));
    }
  });

  this->readSettings();
  this->_recentFiles = new RecentFiles(this, this->_ui->menuRecent, this->_ui->actionClearRecentMenu);

  _ui->mdiArea->setBackground(QImage(":/banner.png"));
  connect(_ui->menuWindow, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

  auto settingsButton = static_cast<QToolButton *>(_ui->mainToolBar->widgetForAction(_ui->actionSettings));
  settingsButton->setPopupMode(QToolButton::ToolButtonPopupMode::MenuButtonPopup);
  settingsButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
  _ui->actionSettings->setMenu(_ui->menuChangeGameSettings);

#ifdef RGM_SERVER_ENABLED
  RGMPlugin *pluginServer = new ServerPlugin(*this);
  auto outputTextBrowser = this->_ui->outputTextBrowser;
  connect(pluginServer, &RGMPlugin::LogOutput, outputTextBrowser, &QTextBrowser::append);
  connect(pluginServer, &RGMPlugin::CompileStatusChanged, [=](bool finished) {
    _ui->outputDockWidget->show();
    _ui->actionRun->setEnabled(finished);
    _ui->actionDebug->setEnabled(finished);
    _ui->actionCreateExecutable->setEnabled(finished);
  });
  connect(this, &MainWindow::CurrentConfigChanged, pluginServer, &RGMPlugin::SetCurrentConfig);
  connect(_ui->actionRun, &QAction::triggered, pluginServer, &RGMPlugin::Run);
  connect(_ui->actionDebug, &QAction::triggered, pluginServer, &RGMPlugin::Debug);
  connect(_ui->actionCreateExecutable, &QAction::triggered, pluginServer, &RGMPlugin::CreateExecutable);
#endif

  openNewProject();
}

MainWindow::~MainWindow() { diagnosticTextEdit = nullptr; delete _ui; }

void MainWindow::setCurrentConfig(const buffers::resources::Settings &settings) {
  emit _instance->CurrentConfigChanged(settings);
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
  settings.setValue("tabbedView", _ui->actionToggleTabbedView->isChecked());
  settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event) {
  _ui->mdiArea->closeAllSubWindows();
  this->writeSettings();
  event->accept();
}

void MainWindow::openSubWindow(MessageModel* res, MainWindow::EditorFactoryFunction factory_function) {
  using namespace google::protobuf;

  auto swIt = _subWindows.find(res);
  QMdiSubWindow *subWindow;
  if (swIt == _subWindows.end() || !*swIt) {
    BaseEditor *editor = factory_function(res, this);

    // TODO: move all these connections into the model wrapper
    // connect(editor, &BaseEditor::ResourceRenamed, resourceMap.get(), &ResourceModelMap::ResourceRenamed);
    // connect(editor, &BaseEditor::ResourceRenamed, [=]() { treeModel->dataChanged(QModelIndex(), QModelIndex()); });
    // connect(treeModel.get(), &TreeModel::ItemRenamed, editor,
    //         [res](TreeModel::Node */*node*/, const QString & /*oldName*/, const QString & /*newName*/) {
    //           const QModelIndex index = res->index(TreeNode::kNameFieldNumber);
    //           emit res->DataChanged(index, index);
    //         });

    subWindow = _subWindows[res] = _ui->mdiArea->addSubWindow(editor);
    subWindow->resize(subWindow->frameSize().expandedTo(editor->size()));
    editor->setParent(subWindow);

    subWindow->connect(subWindow, &QObject::destroyed, [=]() { _subWindows.remove(res); });

    subWindow->setWindowIcon(subWindow->widget()->windowIcon());
    editor->setWindowTitle(
        res->GetParentModel<MessageModel>()->Data(FieldPath::Of<TreeNode>(TreeNode::kNameFieldNumber)).toString());
  } else {
    subWindow = *swIt;
  }

  subWindow->show();
  _ui->mdiArea->setActiveSubWindow(subWindow);
}

void MainWindow::updateWindowMenu() {
  static QList<QAction *> windowActions;
  foreach (auto action, windowActions) {
    _ui->menuWindow->removeAction(action);
    windowActions.removeOne(action);
  }
  auto windows = _ui->mdiArea->subWindowList();
  for (int i = 0; i < windows.size(); ++i) {
    QMdiSubWindow *mdiSubWindow = windows.at(i);

    const auto windowTitle = mdiSubWindow->windowTitle();
    QString numberString = QString::number(i + 1);
    numberString = numberString.insert(numberString.length() - 1, '&');
    QString text = tr("%1 %2").arg(numberString, windowTitle);

    QAction *action = _ui->menuWindow->addAction(
        mdiSubWindow->windowIcon(), text, mdiSubWindow,
          [this, mdiSubWindow]() { _ui->mdiArea->setActiveSubWindow(mdiSubWindow); });
    windowActions.append(action);
    action->setCheckable(true);
    action->setChecked(mdiSubWindow == _ui->mdiArea->activeSubWindow());
  }
}

void MainWindow::openFile(QString fName) {
  QFileInfo fileInfo(fName);

  std::unique_ptr<buffers::Project> loadedProject = egm::LoadProject(fName.toStdString());

  if (!loadedProject) {
    QMessageBox::warning(this, tr("Failed To Open Project"), tr("There was a problem loading the project: ") + fName,
                         QMessageBox::Ok);
    return;
  }

  MainWindow::setWindowTitle(fileInfo.fileName() + " - ENIGMA");
  _recentFiles->prependFile(fName);
  openProject(std::move(loadedProject));
}

void MainWindow::openNewProject() {
  MainWindow::setWindowTitle(tr("<new game> - ENIGMA"));
  auto newProject = std::make_unique<buffers::Project>();
  auto *root = newProject->mutable_game()->mutable_root();
  QList<QString> defaultGroups = {tr("Sprites"), tr("Sounds"),  tr("Backgrounds"), tr("Paths"),
                                  tr("Scripts"), tr("Shaders"), tr("Fonts"),       tr("Timelines"),
                                  tr("Objects"), tr("Rooms"),   tr("Includes"),    tr("Configs")};
  // We can edit the proto directly, here, since the model doesn't exist, yet.
  for (const auto& groupName : defaultGroups) {
    auto *groupNode = root->mutable_folder()->add_children();
    groupNode->set_name(groupName.toStdString());
    groupNode->mutable_folder();
  }
  openProject(std::move(newProject));
}

template<typename Editor> TreeModel::EditorLauncher Launch(MainWindow *parent) {
  struct EditorFactoryFactory {
    static BaseEditor *Factory(MessageModel *model, MainWindow *parent) {
      return new Editor(model, parent);
    }
  };
  return [parent](MessageModel *model) {
    parent->openSubWindow(model, EditorFactoryFactory::Factory);
  };
}

void ConfigureIconFields(ProtoModel::DisplayConfig *conf, const Descriptor *desc, std::set<const Descriptor*> *visited) {
  for (int i = 0; i < desc->field_count(); ++i) {
    const FieldDescriptor *field = desc->field(i);
    if (field->options().HasExtension(buffers::resource_ref)) {
      std::string resource_type = field->options().GetExtension(buffers::resource_ref);
      if (resource_type == "object") {
        conf->SetFieldIconLookup(field, GetObjectSpriteByNameField);
        conf->SetFieldDefaultIcon(field, "object");
      } else if (resource_type == "sprite") {
        conf->SetFieldIconLookup(field, GetSpriteIconByNameField);
      } else if (resource_type == "background") {
        conf->SetFieldIconLookup(field, GetBackgroundIconByNameField);
      } else if (resource_type == "room") {
        // no preview for rooms yet
      } else {
        qDebug() << "Unknown resource ref type: " << resource_type.c_str();
      }
    }
    if (field->options().HasExtension(buffers::file_kind)) {
      if (field->options().GetExtension(buffers::file_kind) == buffers::FileKind::IMAGE) {
        conf->SetFieldIconLookup(field, GetFileIcon);
      }
    }
    if (const Descriptor *submsg = field->message_type()) {
      if (visited->insert(submsg).second) ConfigureIconFields(conf, submsg, visited);
    }
  }
}

void ConfigureIconFields(ProtoModel::DisplayConfig *conf, const Descriptor *desc) {
  std::set<const Descriptor*> visited{desc};
  return ConfigureIconFields(conf, desc, &visited);
}


void MainWindow::openProject(std::unique_ptr<buffers::Project> openedProject) {
  this->_ui->mdiArea->closeAllSubWindows();
  ArtManager::clearCache();

  _project = std::move(openedProject);

  TreeModel::DisplayConfig treeConf;
  treeConf.UseEditorLauncher<buffers::resources::Sprite>(Launch<SpriteEditor>(this));
  treeConf.UseEditorLauncher<buffers::resources::Sound>(Launch<SoundEditor>(this));
  treeConf.UseEditorLauncher<buffers::resources::Background>(Launch<BackgroundEditor>(this));
  treeConf.UseEditorLauncher<buffers::resources::Path>(Launch<PathEditor>(this));
  treeConf.UseEditorLauncher<buffers::resources::Font>(Launch<FontEditor>(this));
  treeConf.UseEditorLauncher<buffers::resources::Script>(Launch<ScriptEditor>(this));
  treeConf.UseEditorLauncher<buffers::resources::Shader>(Launch<ShaderEditor>(this));
  treeConf.UseEditorLauncher<buffers::resources::Timeline>(Launch<TimelineEditor>(this));
  treeConf.UseEditorLauncher<buffers::resources::Object>(Launch<ObjectEditor>(this));
  treeConf.UseEditorLauncher<buffers::resources::Room>(Launch<RoomEditor>(this));
  treeConf.UseEditorLauncher<buffers::resources::Settings>(Launch<SettingsEditor>(this));

  ProtoModel::DisplayConfig msgConf;
  msgConf.SetDefaultIcon<buffers::TreeNode::Folder>("group");
  msgConf.SetDefaultIcon<buffers::TreeNode::UnknownResource>("info");
  msgConf.SetDefaultIcon<buffers::resources::Sprite>("sprite");
  msgConf.SetDefaultIcon<buffers::resources::Sound>("sound");
  msgConf.SetDefaultIcon<buffers::resources::Background>("background");
  msgConf.SetDefaultIcon<buffers::resources::Path>("path");
  msgConf.SetDefaultIcon<buffers::resources::Script>("script");
  msgConf.SetDefaultIcon<buffers::resources::Shader>("shader");
  msgConf.SetDefaultIcon<buffers::resources::Font>("font");
  msgConf.SetDefaultIcon<buffers::resources::Timeline>("timeline");
  msgConf.SetDefaultIcon<buffers::resources::Object>("object");
  msgConf.SetDefaultIcon<buffers::resources::Room>("room");
  msgConf.SetDefaultIcon<buffers::resources::Settings>("settings");

  ConfigureIconFields(&msgConf, TreeNode::GetDescriptor());

  msgConf.SetMessageIconPathField<buffers::resources::Sprite>(
        FieldPath::RepeatedOffset(buffers::resources::Sprite::kSubimagesFieldNumber, 0));
  msgConf.SetMessageIconPathField<buffers::resources::Background>(buffers::resources::Background::kImageFieldNumber);
  msgConf.SetMessageIconIdLookup<buffers::resources::Object>(GetSpriteIconByNameField,
                                                              buffers::resources::Object::kSpriteNameFieldNumber);

  msgConf.SetMessageLabelField<buffers::TreeNode>(buffers::TreeNode::kNameFieldNumber);

  msgConf.SetFieldHeaderIcon<buffers::resources::Path::Point>(":/actions/diamond-red.png",
                                                              buffers::resources::Path::Point::kXFieldNumber);
  msgConf.SetFieldHeaderIcon<buffers::resources::Path::Point>(":/actions/diamond-green.png",
                                                              buffers::resources::Path::Point::kYFieldNumber);
  msgConf.SetFieldHeaderIcon<buffers::resources::Path::Point>(":/actions/diamond-blue.png",
                                                              buffers::resources::Path::Point::kSpeedFieldNumber);

  treeConf.SetMessagePassthrough<buffers::TreeNode>();
  treeConf.SetMessagePassthrough<buffers::TreeNode::Folder>();
  treeConf.DisableOneofReassignment<buffers::TreeNode>();

  resourceMap.reset(new ResourceModelMap(_project->mutable_game()->mutable_root(), nullptr));

  auto pm = new MessageModel(this, _project->mutable_game()->mutable_root());
  pm->SetDisplayConfig(msgConf);

  treeModel.reset(new TreeModel(pm, nullptr, treeConf));

  _ui->treeView->setModel(treeModel.get());
  treeModel->connect(treeModel.get(), &TreeModel::ItemRenamed, resourceMap.get(),
                     &ResourceModelMap::ResourceRenamed);
}

void MainWindow::on_actionNew_triggered() { openNewProject(); }

void MainWindow::on_actionOpen_triggered() {
  const QString &fileName = QFileDialog::getOpenFileName(
      this, tr("Open Project"), "",
      tr("All supported formats (*.egm *.yyp *.project.gmx *.gm81 *.gmk *.gm6 *.gmd);;GameMaker: Studio 2 Projects "
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
  _ui->actionToggleTabbedView->setChecked(enabled);
  _ui->mdiArea->setViewMode(enabled ? QMdiArea::TabbedView : QMdiArea::SubWindowView);
  if (enabled) {
    QTabBar *tabBar = _ui->mdiArea->findChild<QTabBar *>();
    if (tabBar) {
      tabBar->setExpanding(false);
    }
  }
}

void MainWindow::on_actionCascade_triggered() {
  this->setTabbedMode(false);
  _ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_actionTile_triggered() {
  this->setTabbedMode(false);
  _ui->mdiArea->tileSubWindows();
}

void MainWindow::on_actionCloseAll_triggered() { _ui->mdiArea->closeAllSubWindows(); }

void MainWindow::on_actionCloseOthers_triggered() {
  foreach (QMdiSubWindow *subWindow, _ui->mdiArea->subWindowList()) {
    if (subWindow != _ui->mdiArea->activeSubWindow()) subWindow->close();
  }
}

void MainWindow::on_actionToggleTabbedView_triggered() { this->setTabbedMode(_ui->actionToggleTabbedView->isChecked()); }

void MainWindow::on_actionNext_triggered() { _ui->mdiArea->activateNextSubWindow(); }

void MainWindow::on_actionPrevious_triggered() { _ui->mdiArea->activatePreviousSubWindow(); }

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
                       tr("ENIGMA is a free, open-source, and cross-platform game engine."),
                       QMessageBox::Ok, this);
  QAbstractButton *aboutQtButton = aboutBox.addButton(tr("About Qt"), QMessageBox::HelpRole);
  aboutBox.exec();

  if (aboutBox.clickedButton() == aboutQtButton) {
    QMessageBox::aboutQt(this, tr("About Qt"));
  }
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index) {
  if (treeModel->rowCount(index)) {
    // Allow node expansion to happen.
    return;
  }
  treeModel->triggerNodeEdit(index, _ui->treeView);
}

void MainWindow::on_actionClearRecentMenu_triggered() { _recentFiles->clear(); }

void MainWindow::CreateResource(TypeCase typeCase) {
  TreeNode child;
  auto fieldNum = ResTypeFields[typeCase];
  const Descriptor *desc = child.GetDescriptor();
  const Reflection *refl = child.GetReflection();
  const FieldDescriptor *field = desc->FindFieldByNumber(fieldNum);

  // allocate and set the child's resource field
  refl->MutableMessage(&child, field);

  // find a unique name for the new resource
  child.set_name(resourceMap->CreateResourceName(&child).toStdString());
  // release ownership of the new child to its parent and the tree
  auto index = this->treeModel->addNode(child, _ui->treeView->currentIndex());
  treeModel->triggerNodeEdit(index, _ui->treeView);
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
  // if (!_ui->treeView->selectionModel()->hasSelection()) return;
  const auto index = _ui->treeView->selectionModel()->currentIndex();
  QModelIndex dupIndex = treeModel->duplicateNode(index);
  // Triggers edit of either resource or name label.
  // TODO: maybe confirm before duplicating an entire fucking group; we're all reasonable people
  treeModel->triggerNodeEdit(dupIndex, _ui->treeView);
}

void MainWindow::on_actionCreateGroup_triggered() {
  TreeNode child;
  child.mutable_folder();  // Gives the node an empty folder.

  // find a unique name for the new group
  const QString name = resourceMap->CreateResourceName(TypeCase::kFolder, "group");
  child.set_name(name.toStdString());

  this->treeModel->triggerNodeEdit(this->treeModel->addNode(child, _ui->treeView->currentIndex()), _ui->treeView);
}

void MainWindow::on_actionRename_triggered() {
  if (!_ui->treeView->selectionModel()->hasSelection()) return;
  _ui->treeView->edit(_ui->treeView->selectionModel()->currentIndex());
}

void MainWindow::on_actionProperties_triggered() {
  if (!_ui->treeView->selectionModel()->hasSelection()) return;
  auto selected = _ui->treeView->selectionModel()->selectedIndexes();
  for (auto index : selected) {
    treeModel->triggerNodeEdit(index, _ui->treeView);
  }
}

/*static void CollectNodes(const buffers::TreeNode *root, QSet<const buffers::TreeNode *> &cache) {
  cache.insert(root);
  for (int i = 0; i < root->folder().children_size(); ++i) {
    auto *child = &root->folder().children(i);
    cache.insert(child);
    if (child->has_folder()) CollectNodes(child, cache);
  }
}*/

void MainWindow::on_actionDelete_triggered() {/*
  if (!_ui->treeView->selectionModel()->hasSelection()) return;
  auto selected = _ui->treeView->selectionModel()->selectedIndexes();
  QSet<MessageModel *> deleted_models;
  for (auto index : selected) {
    auto *treeNode = static_cast<buffers::TreeNode *>(index.internalPointer());
    CollectNodes(treeNode, selectedNodes);
  }
  QString selectedNames = "";
  for (auto node : selectedNodes) {
    selectedNames += (node == *selectedNodes.begin() ? "" : ", ") + QString::fromStdString(node->name());
  }

  QMessageBox mb(
    QMessageBox::Icon::Question,
    tr("Delete Resources"),
    tr("Do you want to delete the selected resources from the project?"),
    QMessageBox::Yes | QMessageBox::No, this
  );
  mb.setDetailedText(selectedNames);
  int ret = mb.exec();
  if (ret != QMessageBox::Yes) return;

  // close subwindows
  for (auto node : selectedNodes) {
    if (_subWindows.contains(node)) _subWindows[node]->close();
  }

  // remove tree nodes (recursively unmaps names)
  for (auto index : selected) {
    this->treeModel->removeNode(index);
  }
*/}

void MainWindow::on_actionExpand_triggered() { _ui->treeView->expandAll(); }

void MainWindow::on_actionCollapse_triggered() { _ui->treeView->collapseAll(); }

void MainWindow::on_actionSortByName_triggered() {
  if (!_ui->treeView->selectionModel()->hasSelection()) return;
  treeModel->sortByName(_ui->treeView->currentIndex());
}

void MainWindow::on_treeView_customContextMenuRequested(const QPoint &pos) {
  _ui->menuEdit->exec(_ui->treeView->mapToGlobal(pos));
}
