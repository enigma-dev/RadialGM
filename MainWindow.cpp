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
#include "Components/EditorServices.h"

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
#include <sstream>

#undef GetMessage

QList<QString> MainWindow::EnigmaSearchPaths = {QDir::currentPath(), "./enigma-dev",
                                                "../RadialGM/Submodules/enigma-dev", "../enigma-dev"};
QFileInfo MainWindow::EnigmaRoot = MainWindow::getEnigmaRoot();
QList<buffers::SystemType> MainWindow::systemCache;
MainWindow *MainWindow::_instance = nullptr;
QScopedPointer<ProtoModel> MainWindow::protoModel;
QScopedPointer<TreeModel> MainWindow::treeModel;
QScopedPointer<DiagnosticModel> MainWindow::diagModel;

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
  QString msgFormatted = msgFormat.arg(typeName, localMsg.constData(), file,
                                       QString::number(context.line), function);

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
    auto entryList = dir.entryInfoList(QStringList({"ENIGMAsystem"}), filters,
                                       QDir::SortFlag::NoSort);
    if (!entryList.empty()) {
      EnigmaRoot = entryList.first();
      break;
    }
  }

  return EnigmaRoot;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _gameModified(false),
  _ui(new Ui::MainWindow), _event_data(nullptr), egm(nullptr) {
  auto eventsPath = (EnigmaRoot.absolutePath() + "/events.ey");
  QFile eventsFile(eventsPath);
  if (eventsFile.exists()) {
    _event_data = std::make_unique<EventData>(ParseEventFile(eventsPath.toStdString()));
  } else {
    eventsFile.setFileName("/events.ey");
    qDebug() << "Error: Failed to locate ENIGMA sources. Loading internal events.ey.\n"
             << "Search Paths:\n" << MainWindow::EnigmaSearchPaths;
    QFile internal_events(":/events.ey");
    internal_events.open(QIODevice::ReadOnly | QFile::Text);
    std::stringstream ss;
    ss << internal_events.readAll().toStdString();
    _event_data = std::make_unique<EventData>(ParseEventFile(ss));
  }

  egm = egm::EGM(_event_data.get());

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
  if (!showSaveConfirmation()) {
    event->ignore();
    return;
  }

  this->writeSettings();
  event->accept();
}

using EditorFactory = std::function<BaseEditor *(EditorModel * m, QWidget * p)>;
template <typename T>
T *editorFactory(EditorModel *model, QWidget *parent) {
  return new T(model, parent);
}

void MainWindow::openEditor(const QModelIndex& protoIndex) {
  using namespace google::protobuf;

  using TypeCase = buffers::TreeNode::TypeCase;
  using FactoryMap = std::unordered_map<TypeCase,EditorFactory>;

  static FactoryMap factoryMap({{TypeCase::kSprite, editorFactory<SpriteEditor>},
                                {TypeCase::kSound, editorFactory<SoundEditor>},
                                {TypeCase::kBackground, editorFactory<BackgroundEditor>},
                                {TypeCase::kPath, editorFactory<PathEditor>},
                                {TypeCase::kFont, editorFactory<FontEditor>},
                                {TypeCase::kScript, editorFactory<ScriptEditor>},
                                {TypeCase::kShader, editorFactory<ShaderEditor>},
                                {TypeCase::kTimeline, editorFactory<TimelineEditor>},
                                {TypeCase::kObject, editorFactory<ObjectEditor>},
                                {TypeCase::kRoom, editorFactory<RoomEditor>},
                                {TypeCase::kSettings, editorFactory<SettingsEditor>}});

  auto swIt = _editors.find(protoIndex);
  QMdiSubWindow *subWindow;
  if (swIt == _editors.end() || !*swIt) {
    auto ptr = protoModel->data(protoIndex,Qt::UserRole+1).value<quintptr>();
    auto item = reinterpret_cast<TreeNode*>(ptr);
    auto factoryFunction = factoryMap.find(item->type_case());
    if (factoryFunction == factoryMap.end()) return;  // no registered editor

    // editors proxy the super model from their tree index
    EditorModel *model = new EditorModel(protoIndex, nullptr);
    // we don't give editors direct access to the super model for safety
    model->setSourceModel(protoModel.get());
    // ths base editor will now take over ownership of the proxy model
    BaseEditor *editor = factoryFunction->second(model, this);
    // register the editor with some external editing services
    connect(editor, &BaseEditor::OpenExternally,
            [editor,model](const QModelIndex& editorIndex) {
      auto protoIndex = model->mapToSource(editorIndex);
      EditorServices::OpenExternally(protoIndex);
    });
    connect(editor, &BaseEditor::EditExternally,
            [editor,model](const QModelIndex& editorIndex) {
      auto protoIndex = model->mapToSource(editorIndex);
      EditorServices::EditExternally(protoIndex);
    });

    subWindow = _editors[protoIndex] = _ui->mdiArea->addSubWindow(editor);
    subWindow->resize(subWindow->frameSize().expandedTo(editor->size()));
    editor->setParent(subWindow);

    QPersistentModelIndex persistent = protoIndex;
    subWindow->connect(subWindow, &QObject::destroyed, [=]() {
      _editors.remove(persistent);
    });

    subWindow->setWindowIcon(subWindow->widget()->windowIcon());
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
    QString text = tr("%1 %2").arg(numberString).arg(windowTitle);

    QAction *action = _ui->menuWindow->addAction(
        mdiSubWindow->windowIcon(), text, mdiSubWindow,
          [this, mdiSubWindow]() { _ui->mdiArea->setActiveSubWindow(mdiSubWindow); });
    windowActions.append(action);
    action->setCheckable(true);
    action->setChecked(mdiSubWindow == _ui->mdiArea->activeSubWindow());
  }
}

void MainWindow::openFile(QString fName) {
  if (!showSaveConfirmation()) return;

  QFileInfo fileInfo(fName);
  const QString suffix = fileInfo.suffix();

  std::unique_ptr<buffers::Project> loadedProject = nullptr;
  if (suffix == "egm") {
    loadedProject = egm.LoadEGM(fName.toStdString());
  } else if (suffix == "gm81" || suffix == "gmk" || suffix == "gm6" || suffix == "gmd") {
    loadedProject = gmk::LoadGMK(fName.toStdString(), _event_data.get());
  } else if (suffix == "gmx") {
    loadedProject = gmx::LoadGMX(fName.toStdString(), _event_data.get());
  } else if (suffix == "yyp") {
    loadedProject = yyp::LoadYYP(fName.toStdString(), _event_data.get());
  }

  if (!loadedProject) {
    QMessageBox::warning(this, tr("Failed To Open Project"),tr("There was a problem loading the project: ") + fName,
                         QMessageBox::Ok);
    return;
  }

  MainWindow::setWindowTitle(fileInfo.fileName() + "[*] - ENIGMA");
  _recentFiles->prependFile(fName);
  openProject(std::move(loadedProject));
}

void MainWindow::openNewProject() {
  if (!showSaveConfirmation()) return;

  setWindowTitle(tr("<new game>[*] - ENIGMA"));
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
  ArtManager::clearCache();

  _project = std::move(openedProject);

  protoModel.reset(new ProtoModel(nullptr, _project.get()));
  // anybody who fucks with the super model also
  // fucks with the main window so let them know
  SetGameModified(false);
  auto markDirty = [this]() {
    SetGameModified(true);
  };
  // handle fields being changed
  connect(protoModel.get(), &ProtoModel::dataChanged, markDirty);
  // handle repeated fields being changed (e.g, insert/move/remove)
  connect(protoModel.get(), &ProtoModel::rowsInserted, markDirty);
  connect(protoModel.get(), &ProtoModel::rowsMoved, markDirty);
  connect(protoModel.get(), &ProtoModel::rowsRemoved, markDirty);

  if (treeModel.isNull()) // << construct it
    treeModel.reset(new TreeModel(protoModel.get(), nullptr));
  else // << just update its source instead
    treeModel->setSourceModel(protoModel.get());

  if (diagModel.isNull()) // << construct it
    diagModel.reset(new DiagnosticModel(protoModel.get(), nullptr));
  else // << update source/cascades to inspector & sort filter automagically
    diagModel->setSourceModel(protoModel.get());

  _ui->treeView->setModel(treeModel.get());
  treeModel->connect(_ui->treeFilterEdit, &QLineEdit::textChanged, treeModel.get(),
                     static_cast<void (TreeModel::*)(const QString&)>(
                       &TreeModel::setFilterRegularExpression));
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

bool MainWindow::showSaveConfirmation() {
  if (isWindowModified()) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Project Changed"),
                                  tr("Do you want to save the changes?"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
      return false;
    } else if (reply == QMessageBox::Yes) {
      //TODO: show EGM save dialog saveProject here
    }
  }

  this->closeSubWindows();
  return true;
}

void MainWindow::closeSubWindows() {
  // closes subwindows ignoring their window modified
  auto mdiArea = _ui->mdiArea;
  auto subwindows = mdiArea->subWindowList();
  foreach(auto subwindow, subwindows) {
    subwindow->widget()->setWindowModified(false);
  }
  _ui->mdiArea->closeAllSubWindows();
}

void MainWindow::on_actionCascade_triggered() {
  this->setTabbedMode(false);
  _ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_actionTile_triggered() {
  this->setTabbedMode(false);
  _ui->mdiArea->tileSubWindows();
}

void MainWindow::on_actionCloseAll_triggered() { this->closeSubWindows(); }

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

void MainWindow::on_actionExploreENIGMA_triggered() {
  QDesktopServices::openUrl(QUrl(".", QUrl::TolerantMode));
}

void MainWindow::on_actionShowDiagnosticInspector_triggered() {
  // lazy loaded so it can be reactivated if already shown
  static QDialog *window = nullptr;

  if (window) {
    window->show();
    window->activateWindow();
    return;
  }

  // only QDialog centers on the parent out of the box
  window = new QDialog(this);
  QVBoxLayout *vbl = new QVBoxLayout();

  QLineEdit *filterEdit = new QLineEdit();
  filterEdit->setPlaceholderText(tr("Filter"));

  // the filter model then lets us filter on the additional columns too
  QSortFilterProxyModel *filterModel = new QSortFilterProxyModel(window);
  filterModel->setRecursiveFilteringEnabled(true); // << no pruning
  filterModel->setFilterKeyColumn(-1); // << all columns
  filterModel->setSourceModel(diagModel.get());
  filterModel->connect(filterEdit, &QLineEdit::textChanged, filterModel,
                       static_cast<void (QSortFilterProxyModel::*)(const QString&)>(
                         &QSortFilterProxyModel::setFilterRegularExpression));

  QTreeView *inspectorTable = new QTreeView();
  // we only want to filter not sort
  // we need to see repeated fields in the correct order
  // since it's for diagnostics only
  inspectorTable->setSortingEnabled(false);
  inspectorTable->setModel(filterModel);
  inspectorTable->setDragEnabled(true);
  inspectorTable->setAcceptDrops(true);
  inspectorTable->setDefaultDropAction(Qt::MoveAction);
  inspectorTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
  //inspectorTable->header()->setCascadingSectionResizes(true);
  inspectorTable->header()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
  inspectorTable->header()->setSectionResizeMode(1,QHeaderView::Stretch);

  vbl->addWidget(filterEdit);
  vbl->addWidget(inspectorTable);

  window->setLayout(vbl);
  window->setWindowTitle(tr("Diagnostic Inspector"));
  // give us a maximize button
  window->setWindowFlags(Qt::Window);
  window->show();
}

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
  openEditor(treeModel->mapToSource(index));
}

void MainWindow::on_actionClearRecentMenu_triggered() { _recentFiles->clear(); }

void MainWindow::CreateResource(TypeCase typeCase) {
  // insert us into the proto model through tree proxy
  auto index = treeModel->addNode(_ui->treeView->currentIndex());
  auto ptr = treeModel->data(index,Qt::UserRole+1).value<quintptr>();
  auto child = reinterpret_cast<TreeNode*>(ptr);

  bool is_folder = (typeCase == TypeCase::kFolder);
  if (is_folder) {
    child->set_folder(true);
  } else {
    // allocate and set the child's resource field
    auto desc = child->GetDescriptor();
    auto refl = child->GetReflection();
    auto fieldNum = typeCase;
    auto field = desc->FindFieldByNumber(fieldNum);
    refl->MutableMessage(child, field);
  }

  // find a unique name for the new resource
  //TODO: FIXME
  /*
  const QString name = is_folder ?
      resourceMap->CreateResourceName(TypeCase::kFolder, "group") :
      resourceMap->CreateResourceName(child);*/
  child->set_name("resource");

  // select the new node so that it gets "revealed" and its parent is expanded
  _ui->treeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  // start editing the name of the resource in the tree for convenience
  _ui->treeView->edit(index);
  // open the new resource for editing
  if (!is_folder) openEditor(treeModel->mapToSource(index));
}

void MainWindow::on_actionCreateGroup_triggered() { CreateResource(TypeCase::kFolder); }

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
  if (!_ui->treeView->selectionModel()->hasSelection()) return;
  const auto index = _ui->treeView->selectionModel()->currentIndex();
  const auto *node = static_cast<const buffers::TreeNode *>(index.internalPointer());
  if (node->has_folder()) return;

  //TODO: FIXME
}

void MainWindow::on_actionRename_triggered() {
  if (!_ui->treeView->selectionModel()->hasSelection()) return;
  _ui->treeView->edit(_ui->treeView->selectionModel()->currentIndex());
}

void MainWindow::on_actionProperties_triggered() {
  if (!_ui->treeView->selectionModel()->hasSelection()) return;
  auto selected = _ui->treeView->selectionModel()->selectedIndexes();
  for (auto index : selected) {
    openEditor(treeModel->mapToSource(index));
  }
}

void MainWindow::on_actionDelete_triggered() {
  if (!_ui->treeView->selectionModel()->hasSelection()) return;
  auto selected = _ui->treeView->selectionModel()->selectedIndexes();

  QString selectedNames = "";
  for (auto index : selected) {
    auto name = treeModel->data(index, Qt::DisplayRole);
    selectedNames += (index == *selected.begin() ? "" : ", ") + name.toString();
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
  //TODO: FIXME
  //for (auto node : selectedNodes) {
    //if (_subWindows.contains(node)) _subWindows[node]->close();
  //}

  // remove tree nodes (recursively unmaps names)
  //TODO: FIXME
  //QList<QPersistentModelIndex> persistent = selected.value();
  for (auto index : selected)
    this->treeModel->removeRow(index.row(),index.parent());
}

void MainWindow::on_actionExpand_triggered() { _ui->treeView->expandAll(); }

void MainWindow::on_actionCollapse_triggered() { _ui->treeView->collapseAll(); }

void MainWindow::on_actionSortByName_triggered() {
  if (!_ui->treeView->selectionModel()->hasSelection()) return;
  treeModel->sort(_ui->treeView->currentIndex());
}

void MainWindow::on_treeView_customContextMenuRequested(const QPoint &pos) {
  _ui->menuEdit->exec(_ui->treeView->mapToGlobal(pos));
}
