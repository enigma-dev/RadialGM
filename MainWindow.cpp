#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Dialogs/PreferencesDialog.h"

#include "Editors/BackgroundEditor.h"
#include "Editors/FontEditor.h"
#include "Editors/ObjectEditor.h"
#include "Editors/PathEditor.h"
#include "Editors/RoomEditor.h"
#include "Editors/SpriteEditor.h"
#include "Editors/TimelineEditor.h"

#include "Components/ArtManager.h"

#include "Plugins/RGMPlugin.h"
#include "Plugins/ServerPlugin.h"

#include "gmx.h"

#include "resources/Background.pb.h"

#include <QtWidgets>

#include <functional>

#undef GetMessage

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ArtManager::Init();
  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  ui->setupUi(this);

  ui->mdiArea->setBackground(QImage(":/banner.png"));

  RGMPlugin *pluginServer = new ServerPlugin(*this);
  auto outputTextBrowser = this->ui->outputTextBrowser;
  connect(pluginServer, &RGMPlugin::OutputRead, outputTextBrowser, &QTextBrowser::append);
  connect(pluginServer, &RGMPlugin::ErrorRead, outputTextBrowser, &QTextBrowser::append);
  connect(ui->actionRun, &QAction::triggered, pluginServer, &RGMPlugin::Run);
  connect(ui->actionDebug, &QAction::triggered, pluginServer, &RGMPlugin::Debug);
  connect(ui->actionCreateExecutable, &QAction::triggered, pluginServer, &RGMPlugin::CreateExecutable);
}

void MainWindow::closeEvent(QCloseEvent *event) { QMainWindow::closeEvent(event); }

template <typename T>
T *EditorFactory(QWidget *parent, ProtoModel *model) {
  return new T(parent, model);
}

void MainWindow::openSubWindow(buffers::TreeNode *item) {
  using namespace google::protobuf;

  using TypeCase = buffers::TreeNode::TypeCase;
  using FactoryFunction = std::function<BaseEditor *(QWidget * parent, ProtoModel * model)>;
  using FactoryMap = std::unordered_map<TypeCase, FactoryFunction>;

  static FactoryMap factoryMap({{TypeCase::kSprite, EditorFactory<SpriteEditor>},
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
  // might not be set or its not a message
  if (!typeField || typeField->cpp_type() != FieldDescriptor::CppType::CPPTYPE_MESSAGE) return;
  Message *typeMessage = refl->MutableMessage(item, typeField);

  auto subWindow = subWindows[item];
  if (!subWindows.contains(item) || subWindow == nullptr) {
    auto factoryFunction = factoryMap.find(item->type_case());
    if (factoryFunction == factoryMap.end()) return;  // no registered editor

    if (!resourceModels.contains(item)) resourceModels[item] = new ProtoModel(typeMessage);
    auto resourceModel = resourceModels[item];

    QWidget *editor = factoryFunction->second(ui->mdiArea, resourceModel);
    resourceModel->setParent(editor);

    subWindow = subWindows[item] = ui->mdiArea->addSubWindow(editor);
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

MainWindow::~MainWindow() { delete ui; }

void MainWindow::openFile(QString fName) {
  QFileInfo fileInfo(fName);
  this->setWindowTitle(fileInfo.fileName() + " - ENIGMA");
  project = gmx::LoadGMX(fName.toStdString());
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
  const QString &fileName = QFileDialog::getOpenFileName(this, tr("Open Project"), "",
                                                         tr("GameMaker: Studio (*.project.gmx);;All Files (*)"));
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
  QUrl documentationURL("https://enigma-dev.org/docs/Wiki/Main_Page", QUrl::TolerantMode);
  QDesktopServices::openUrl(documentationURL);
}

void MainWindow::on_actionWebsite_triggered() {
  QUrl websiteURL("https://enigma-dev.org", QUrl::TolerantMode);
  QDesktopServices::openUrl(websiteURL);
}

void MainWindow::on_actionCommunity_triggered() {
  QUrl communityURL("https://enigma-dev.org/forums/", QUrl::TolerantMode);
  QDesktopServices::openUrl(communityURL);
}

void MainWindow::on_actionSubmitIssue_triggered() {
  QUrl submitIssueURL("https://github.com/enigma-dev/RadialGM/issues", QUrl::TolerantMode);
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
