#include "MainWindow.h"
#include "ArtManager.h"
#include "ui_MainWindow.h"

#include "Dialogs/PreferencesDialog.h"

#include "Editors/BackgroundEditor.h"
#include "Editors/FontEditor.h"
#include "Editors/ObjectEditor.h"
#include "Editors/PathEditor.h"
#include "Editors/RoomEditor.h"
#include "Editors/TimelineEditor.h"
#include "gmx.h"

#include "Editors/ResourceModel.h"
#include "resources/Background.pb.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ArtManager::Init();
  ui->setupUi(this);
  ui->mdiArea->setBackground(QImage(":/banner.png"));
}

void MainWindow::openSubWindow(buffers::TreeNode *item) {
  if (item->has_background()) {
    if (!resourceModels.contains(item)) resourceModels[item] = new ResourceModel(item->mutable_background());

	if (!subWindows.contains(item))
      subWindows[item] = ui->mdiArea->addSubWindow(new BackgroundEditor(this, resourceModels[item]));
  } else if (item->has_font()) {
    if (!resourceModels.contains(item)) resourceModels[item] = new ResourceModel(item->mutable_font());

    if (!subWindows.contains(item))
      subWindows[item] = ui->mdiArea->addSubWindow(new FontEditor(this, resourceModels[item]));
  } else if (item->has_object()) {
    if (!resourceModels.contains(item)) resourceModels[item] = new ResourceModel(item->mutable_object());

    if (!subWindows.contains(item))
      subWindows[item] = ui->mdiArea->addSubWindow(new ObjectEditor(this, resourceModels[item]));
  } else if (item->has_path()) {
    if (!resourceModels.contains(item)) resourceModels[item] = new ResourceModel(item->mutable_path());

    if (!subWindows.contains(item))
      subWindows[item] = ui->mdiArea->addSubWindow(new PathEditor(this, resourceModels[item]));
  } else if (item->has_room()) {
    if (!resourceModels.contains(item)) resourceModels[item] = new ResourceModel(item->mutable_room());

    if (!subWindows.contains(item))
      subWindows[item] = ui->mdiArea->addSubWindow(new RoomEditor(this, resourceModels[item]));
  } else if (item->has_script()) {
    /*if (!resourceModels.contains(item))
            resourceModels[item] = new ResourceModel(item->mutable_background());

        if (!subWindows.contains(item))1
            subWindows[item] = ui->mdiArea->addSubWindow(new ScriptEditor(this, resourceModels[item]));*/
  } else if (item->has_shader()) {
    /*if (!resourceModels.contains(item))
            resourceModels[item] = new ResourceModel(item->mutable_shader());

        if (!subWindows.contains(item))
            subWindows[item] = ui->mdiArea->addSubWindow(new ShaderEditor(this, resourceModels[item]));*/
  } else if (item->has_sound()) {
    /*if (!resourceModels.contains(item))
            resourceModels[item] = new ResourceModel(item->mutable_sound());

        if (!subWindows.contains(item))
            subWindows[item] = ui->mdiArea->addSubWindow(new SoundEditor(this, resourceModels[item]));*/
  } else if (item->has_sprite()) {
    /*if (!resourceModels.contains(item))
            resourceModels[item] = new ResourceModel(item->mutable_sprite());

        if (!subWindows.contains(item))
            subWindows[item] = ui->mdiArea->addSubWindow(new SpriteEditor(this, resourceModels[item]));*/
  } else if (item->has_timeline()) {
    if (!resourceModels.contains(item)) resourceModels[item] = new ResourceModel(item->mutable_timeline());

    if (!subWindows.contains(item))
      subWindows[item] = ui->mdiArea->addSubWindow(new TimelineEditor(this, resourceModels[item]));
  }

  auto subWindow = subWindows[item];
  if (subWindow == nullptr) return;
  subWindow->connect(subWindow, &QObject::destroyed, [=]() { subWindows.remove(item); });
  subWindow->setWindowIcon(subWindows[item]->widget()->windowIcon());
  subWindow->setWindowTitle(QString::fromStdString(item->name()));
  subWindow->show();
  subWindow->raise();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::openFile(QString fName) {
  game = gmx::LoadGMX(fName.toStdString(), false);
  tree = new TreeModel(game->mutable_game()->mutable_root(), this);
  ui->treeView->setModel(tree);
}

void MainWindow::on_actionOpen_triggered() {
  const QString fileName = QFileDialog::getOpenFileName(this, tr("Open Project"), "",
                                                        tr("ENIGMA (*.egm);;GameMaker: Studio (*.gmx);;All Files (*)"));

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
                       tr("ENIGMA is a free, open-source, and cross-platform game engine."), QMessageBox::Ok, this, 0);
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
