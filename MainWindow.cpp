#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ArtManager.h"

#include "Dialogs/PreferencesDialog.h"

#include "Editors/BackgroundEditor.h"
#include "Editors/ObjectEditor.h"
#include "Editors/FontEditor.h"
#include "Editors/TimelineEditor.h"
#include "Editors/RoomEditor.h"
#include "gmx.h"

#include "Editors/ResourceModel.h"
#include "resources/Background.pb.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
    ArtManager::Init();
    ui->setupUi(this);
    ui->mdiArea->setBackground(QImage(":/banner.png"));
}

void MainWindow::openSubWindow(buffers::TreeNode* item) {
    if (!resourceModels.contains(item)) {
        if (item->has_background()) {
            resourceModels[item] = new ResourceModel(item->mutable_background());
        }
        else if (item->has_font()) {
            resourceModels[item] = new ResourceModel(item->mutable_font());
        }
        else if (item->has_object()) {
            resourceModels[item] = new ResourceModel(item->mutable_object());
        }
        else if (item->has_path()) {
            resourceModels[item] = new ResourceModel(item->mutable_path());
        }
        else if (item->has_room()) {
            resourceModels[item] = new ResourceModel(item->mutable_room());
        }
        else if (item->has_script()) {
            resourceModels[item] = new ResourceModel(item->mutable_background());
        }
        else if (item->has_shader()) {
            resourceModels[item] = new ResourceModel(item->mutable_shader());
        }
        else if (item->has_sound()) {
            resourceModels[item] = new ResourceModel(item->mutable_sound());
        }
        else if (item->has_sprite()) {
            resourceModels[item] = new ResourceModel(item->mutable_sprite());
        }
        else if (item->has_timeline()) {
            resourceModels[item] = new ResourceModel(item->mutable_timeline());
        }
    }

    QWidget* editorWidget;
    if (item->has_background()) {
        editorWidget = new BackgroundEditor(this, resourceModels[item]);
        //return;
    }
    if (item->has_font()) {
        //openSubWindow(new FontEditor(this));
        //return;
    }
    if (item->has_object()) {
        //openSubWindow(new ObjectEditor(this));
        //return;
    }
    if (item->has_path()) {
       // openSubWindow(new PathEditor(this));
        //return;
    }
    if (item->has_room()) {
        //openSubWindow(new RoomEditor(this));
        //return;
    }
    if (item->has_script()) {
        //openSubWindow(new ScriptEditor(this));
        //return;
    }
    if (item->has_shader()) {
        //openSubWindow(new ShaderEditor(this));
        //return;
    }
    if (item->has_sound()) {
        //openSubWindow(new SoundEditor(this));
        //return;
    }
    if (item->has_sprite()) {
        //openSubWindow(new SpriteEditor(this));
        //return;
    }
    if (item->has_timeline()) {
        //openSubWindow(new TimelineEditor(this));
        //return;
}

    QMdiSubWindow* subWindow = ui->mdiArea->addSubWindow(editorWidget);
    subWindow->setWindowIcon(editorWidget->windowIcon());
    subWindow->setWindowTitle(QString::fromStdString(item->name()));
    subWindow->show();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::openFile(QString fName) {
    game = gmx::LoadGMX(fName.toStdString(), false);
    tree = new TreeModel(game->mutable_game()->mutable_root(), this);
    ui->treeView->setModel(tree);
}

void MainWindow::on_actionOpen_triggered()
{
	const QString fileName = QFileDialog::getOpenFileName(
		this,
		tr("Open Project"),
		"",
		tr("ENIGMA (*.egm);;GameMaker: Studio (*.gmx);;All Files (*)")
	);

    if (!fileName.isEmpty())
        openFile(fileName);
}

void MainWindow::on_actionPreferences_triggered()
{
	PreferencesDialog preferencesDialog(this);
	preferencesDialog.exec();
}

void MainWindow::on_actionExit_triggered()
{
	QApplication::exit();
}

void MainWindow::on_actionCascade_triggered()
{
	ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_actionTile_triggered()
{
	ui->mdiArea->tileSubWindows();
}

void MainWindow::on_actionCloseAll_triggered()
{
	ui->mdiArea->closeAllSubWindows();
}

void MainWindow::on_actionToggleTabbedView_triggered()
{
	ui->mdiArea->setViewMode(
		ui->actionToggleTabbedView->isChecked() ? QMdiArea::TabbedView : QMdiArea::SubWindowView
	);
}

void MainWindow::on_actionNext_triggered()
{
	ui->mdiArea->activateNextSubWindow();
}

void MainWindow::on_actionPrevious_triggered()
{
	ui->mdiArea->activatePreviousSubWindow();
}

void MainWindow::on_actionDocumentation_triggered()
{
	QUrl documentationURL("https://enigma-dev.org/docs/Wiki/Main_Page", QUrl::TolerantMode);
	QDesktopServices::openUrl(documentationURL);
}

void MainWindow::on_actionWebsite_triggered()
{
	QUrl websiteURL("https://enigma-dev.org", QUrl::TolerantMode);
	QDesktopServices::openUrl(websiteURL);
}

void MainWindow::on_actionCommunity_triggered()
{
	QUrl communityURL("https://enigma-dev.org/forums/", QUrl::TolerantMode);
	QDesktopServices::openUrl(communityURL);
}

void MainWindow::on_actionSubmitIssue_triggered()
{
	QUrl submitIssueURL("https://github.com/enigma-dev/RadialGM/issues", QUrl::TolerantMode);
	QDesktopServices::openUrl(submitIssueURL);
}

void MainWindow::on_actionExploreENIGMA_triggered()
{
	QDesktopServices::openUrl(QUrl(".", QUrl::TolerantMode));
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox aboutBox(
		QMessageBox::Information,
		tr("About"),
		tr("ENIGMA is a free, open-source, and cross-platform game engine."),
		QMessageBox::Ok,
		this,
		0
	);
	QAbstractButton *aboutQtButton = aboutBox.addButton(tr("About Qt"), QMessageBox::HelpRole);
	aboutBox.exec();

	if (aboutBox.clickedButton() == aboutQtButton) {
		QMessageBox::aboutQt(this, tr("About Qt"));
	}
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    buffers::TreeNode *item = static_cast<buffers::TreeNode*>(index.internalPointer());
    const QString name = QString::fromStdString(item->name());

    if (item->has_folder()) {
        return;
    }

    openSubWindow(item);
}
