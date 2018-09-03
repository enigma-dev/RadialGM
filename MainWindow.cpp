#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Dialogs/PreferencesDialog.h"

#include "Editors/BackgroundEditor.h"
#include "Editors/FontEditor.h"
#include "Editors/ObjectEditor.h"
#include "Editors/PathEditor.h"
#include "Editors/RoomEditor.h"
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

#undef GetMessage

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
  settings.endGroup();
}

void MainWindow::writeSettings() {
  QSettings settings;

  // Save window and dock widget location / state for next session
  settings.beginGroup("MainWindow");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("state", saveState());
  settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event) {
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
  using FactoryFunction = std::function<BaseEditor *(ProtoModel * model, QWidget * parent)>;
  using FactoryMap = std::unordered_map<TypeCase, FactoryFunction>;

  static FactoryMap factoryMap({{TypeCase::kSprite, EditorFactory<SpriteEditor>},
                                {TypeCase::kSound, EditorFactory<SoundEditor>},
                                {TypeCase::kBackground, EditorFactory<BackgroundEditor>},
                                {TypeCase::kPath, EditorFactory<PathEditor>},
                                {TypeCase::kFont, EditorFactory<FontEditor>},
                                {TypeCase::kTimeline, EditorFactory<TimelineEditor>},
                                {TypeCase::kObject, EditorFactory<ObjectEditor>},
                                {TypeCase::kRoom, EditorFactory<RoomEditor>}});

  const Descriptor *desc = item->GetDescriptor();
  const Reflection *refl = item->GetReflection();

  const OneofDescriptor *typeOneof = desc->FindOneofByName("type");
  const FieldDescriptor *typeField = refl->GetOneofFieldDescriptor(*item, typeOneof);
  // might not be set or it's not a message
  if (!typeField || typeField->cpp_type() != FieldDescriptor::CppType::CPPTYPE_MESSAGE) return;
  Message *typeMessage = refl->MutableMessage(item, typeField);

  auto subWindow = subWindows[item];
  if (!subWindows.contains(item) || subWindow == nullptr) {
    auto factoryFunction = factoryMap.find(item->type_case());
    if (factoryFunction == factoryMap.end()) return;  // no registered editor

    if (!resourceModels.contains(item)) resourceModels[item] = new ProtoModel(typeMessage, nullptr);
    auto resourceModel = resourceModels[item];

    QWidget *editor = factoryFunction->second(resourceModel, nullptr);
    resourceModel->setParent(editor);

    subWindow = subWindows[item] = ui->mdiArea->addSubWindow(editor);
    subWindow->resize(subWindow->frameSize().expandedTo(editor->size()));
    editor->setParent(subWindow);

    subWindow->connect(subWindow, &QObject::destroyed, [=]() {
      resourceModels.remove(item);
      subWindows.remove(item);
    });
    subWindow->setWindowIcon(subWindow->widget()->windowIcon());
    subWindow->setWindowTitle(QString::fromStdString(item->name()));
  }

  subWindow->show();
  ui->mdiArea->setActiveSubWindow(subWindow);
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

  treeModel = new TreeModel(project->mutable_game()->mutable_root(), this);
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

void MainWindow::on_actionCascade_triggered() { ui->mdiArea->cascadeSubWindows(); }

void MainWindow::on_actionTile_triggered() { ui->mdiArea->tileSubWindows(); }

void MainWindow::on_actionCloseAll_triggered() { ui->mdiArea->closeAllSubWindows(); }

void MainWindow::on_actionToggleTabbedView_triggered() {
  ui->mdiArea->setViewMode(ui->actionToggleTabbedView->isChecked() ? QMdiArea::TabbedView : QMdiArea::SubWindowView);
}

void MainWindow::on_actionNext_triggered() { ui->mdiArea->activateNextSubWindow(); }

void MainWindow::on_actionPrevious_triggered() { ui->mdiArea->activatePreviousSubWindow(); }

void MainWindow::on_actionDocumentation_triggered() {
  QSettings settings;
  const QString &documentationURI =
      settings.value("Preferences/General/documentationURI", "https://enigma-dev.org/docs/Wiki/Main_Page").toString();
  QUrl documentationURL(documentationURI, QUrl::TolerantMode);
  QDesktopServices::openUrl(documentationURL);
}

void MainWindow::on_actionWebsite_triggered() {
  QSettings settings;
  const QString &websiteURI = settings.value("Preferences/General/websiteURI", "https://enigma-dev.org").toString();
  QUrl websiteURL(websiteURI, QUrl::TolerantMode);
  QDesktopServices::openUrl(websiteURL);
}

void MainWindow::on_actionCommunity_triggered() {
  QSettings settings;
  const QString &communityURI =
      settings.value("Preferences/General/communityURI", "https://enigma-dev.org/forums/").toString();
  QUrl communityURL(communityURI, QUrl::TolerantMode);
  QDesktopServices::openUrl(communityURL);
}

void MainWindow::on_actionSubmitIssue_triggered() {
  QSettings settings;
  const QString &submitIssueURI =
      settings.value("Preferences/General/submitIssueURI", "https://github.com/enigma-dev/RadialGM/issues").toString();
  QUrl submitIssueURL(submitIssueURI, QUrl::TolerantMode);
  QDesktopServices::openUrl(submitIssueURL);
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
