#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Dialogs/PreferencesDialog.h"

#include "Editors/BackgroundEditor.h"
#include "Editors/ObjectEditor.h"
#include "Editors/FontEditor.h"
#include "Editors/TimelineEditor.h"
#include "Editors/RoomEditor.h"

#include "gmx.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->mdiArea->setBackground(QImage(":/banner.png"));
	openSubWindow(new BackgroundEditor(this));
	openSubWindow(new BackgroundEditor(this));

	openSubWindow(new ObjectEditor(this));
	openSubWindow(new FontEditor(this));
	openSubWindow(new TimelineEditor(this));
	openSubWindow(new RoomEditor(this));
}

void MainWindow::openSubWindow(QWidget *editor) {
	QMdiSubWindow* subWindow = ui->mdiArea->addSubWindow(editor);
	//subWindow->resize(editor->size());
	subWindow->setWindowIcon(editor->windowIcon());
}

MainWindow::~MainWindow()
{
	delete ui;
}

void AddToTree(const buffers::TreeNode& node, QTreeWidgetItem* tree) {
    for (const buffers::TreeNode& n : node.node()) {
        QTreeWidgetItem *i = new QTreeWidgetItem();
        i->setText(0, QString::fromStdString(n.name()));

        switch(n.type()) {
            case buffers::TreeNode::FOLDER: {
                i->setIcon(0, QIcon(":/resources/group.png"));
                break;
            }
            case buffers::TreeNode::SOUND: {
                i->setIcon(0, QIcon(":/resources/sound.png"));
                break;
            }
            case buffers::TreeNode::SPRITE: {
                i->setIcon(0, QIcon(":/resources/sprite.png"));
                break;
            }
            case buffers::TreeNode::BACKGROUND: {
                i->setIcon(0, QIcon(":/resources/background.png"));
                break;
            }
            case buffers::TreeNode::PATH: {
                i->setIcon(0, QIcon(":/resources/path.png"));
                break;
            }
            case buffers::TreeNode::SCRIPT: {
                i->setIcon(0, QIcon(":/resources/script.png"));
                break;
            }
            case buffers::TreeNode::SHADER: {
                i->setIcon(0, QIcon(":/resources/shader.png"));
                break;
            }
            case buffers::TreeNode::TIMELINE: {
                i->setIcon(0, QIcon(":/resources/timeline.png"));
                break;
            }
            case buffers::TreeNode::FONT: {
                i->setIcon(0, QIcon(":/resources/font.png"));
                break;
            }
        case buffers::TreeNode::OBJECT: {
                i->setIcon(0, QIcon(":/resources/object.png"));
                break;
            }
            case buffers::TreeNode::ROOM: {
                i->setIcon(0, QIcon(":/resources/room.png"));
                break;
            }
            case buffers::TreeNode::HELP: {
                i->setIcon(0, QIcon(":/resources/info.png"));
                break;
            }
            case buffers::TreeNode::CONFIG: {
                i->setIcon(0, QIcon(":/resources/settings.png"));
                break;
            }
            case buffers::TreeNode::INVALID: {
                i->setIcon(0, QIcon(":/resources/info.png"));
                break;
            }
        }

        tree->addChild(i);
        AddToTree(n, i);
    }
}

void MainWindow::on_actionOpen_triggered()
{
	const QString fileName = QFileDialog::getOpenFileName(
		this,
		tr("Open Project"),
		"",
		tr("ENIGMA (*.egm);;GameMaker: Studio (*.gmx);;All Files (*)")
	);

    game = gmx::LoadGMX(fileName.toStdString(), false);

    ui->treeWidget->clear();

    for (const buffers::TreeNode& n : game->treeroot().node()) {
        QTreeWidgetItem *i = new QTreeWidgetItem();
        i->setText(0, QString::fromStdString(n.name()));
        i->setIcon(0, QIcon(":/resources/group.png"));
        ui->treeWidget->addTopLevelItem(i);

        AddToTree(n, i);
    }
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
