#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	// file menu
	void on_actionOpen_triggered();
	void on_actionPreferences_triggered();
	void on_actionExit_triggered();

	// window menu
	void on_actionCascade_triggered();
	void on_actionTile_triggered();
	void on_actionToggleTabbedView_triggered();
	void on_actionCloseAll_triggered();
	void on_actionNext_triggered();
	void on_actionPrevious_triggered();

	// help menu
	void on_actionDocumentation_triggered();
	void on_actionWebsite_triggered();
	void on_actionCommunity_triggered();
	void on_actionSubmitIssue_triggered();
	void on_actionExploreENIGMA_triggered();
	void on_actionAbout_triggered();

private:
	Ui::MainWindow *ui;

	void openSubWindow(QWidget *editor);
};

#endif // MAINWINDOW_H
