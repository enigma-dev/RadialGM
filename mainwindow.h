/**
* @file  mainwindow.h
* @brief Header implementing a class for the main window.
*
* @section License
*
* Copyright (C) 2013 Robert B. Colton
* This file is a part of the LateralGM IDE.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWindow>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QMdiArea>
#include <QDockWidget>
#include <QTreeWidget>
#include <QHeaderView>
#include <QStatusBar>
#include <QLabel>
#include <QTextEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QProgressBar>
#include <QSettings>
#include "widgets/scriptwidget.h"
#include "widgets/shaderwidget.h"
#include "widgets/fontwidget.h"
#include "widgets/pathwidget.h"
#include "widgets/objectwidget.h"
#include "widgets/timelinewidget.h"
#include "widgets/backgroundwidget.h"
#include "widgets/soundwidget.h"
#include "widgets/roomwidget.h"
#include "widgets/spritewidget.h"
#include "widgets/gameinfoeditor.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/preferencesdialog.h"
#include "serializer/projectmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void outputClear(bool clearLog = true, bool clearMessages = true);
    void outputText(QString text);
    void outputLine(QString text);
    void outputMessage(QString origin, QString location, QString description);

public slots:
    void closeApplication();
    void closeEvent(QCloseEvent *event);
    void showLicenseDialog();
    void showAboutDialog();
    void showPreferencesDialog();
    void showOpenDialog();
    void showSaveDialog();
    void showManual();

    void addSprite();
    void addBackground();
    void addSound();
    void addFont();
    void addShader();
    void addTimeline();
    void addObject();
    void addRoom();
    void addPath();
    void addScript();

    void showGameInformation();

    void restoreLayout();
    void cascadeWindows();
    void tileWindows();
    void closeAllWindows();
    void closeWindow();

    void toggleMdiTabs();
    void toggleOutputMessages();
    void toggleOutputLog();
    void toggleHierarchy();

    void clearRecentFiles();

private:

    QToolBar* fileToolbar;
    QAction* newAction;
    QAction* openAction;
    QAction* saveAction;
    QAction* saveAsAction;
    QToolBar* buildToolbar;
    QAction* runAction;
    QAction* debugAction;
    QAction* compileAction;
    QToolBar* resourceToolbar;
    QAction* spriteAction;
    QAction* soundAction;
    QAction* backgroundAction;
    QAction* pathAction;
    QAction* scriptAction;
    QAction* fontAction;
    QAction* timelineAction;
    QAction* objectAction;
    QAction* roomAction;
    QToolBar* settingsToolbar;
    QAction* preferencesAction;
    QAction* gameSettingsAction;
    QAction* gameInformationAction;
    QAction* extensionsAction;
    QAction* manualAction;

    AboutDialog* aboutDialog;
    PreferencesDialog* prefsDialog;

    GameInfoEditor* infoEditor;

    QStatusBar* mainStatusBar;
    QProgressBar* mainProgressBar;
    QMenuBar* mainMenuBar;
    QMenu* fileMenu;
    QMenu* recentFilesMenu;
    QMenu* editMenu;
    QMenu* resourceMenu;
    QMenu* windowMenu;
    QMenu* helpMenu;

    QDockWidget* messagesDock;
    QDockWidget* logDock;
    QTextEdit* logText;
    QTableWidget* messagesTable;
    QDockWidget* hierarchyDock;
    QTreeWidget* hierarchyTree;
    QMdiArea* mainMdiArea;
    QTreeWidgetItem *spriteGroup, *soundGroup, *backgroundGroup, *pathGroup, *scriptGroup, *shaderGroup, *fontGroup, *timelineGroup, *objectGroup, *roomGroup;
    QMap<QTreeWidgetItem *, QList<QTreeWidgetItem*>*> resourceItemMap;

    ProjectManager* currentFile;

    QByteArray defaultState;

    QTreeWidgetItem *addResourceGroup(QString name);
    void addResource(QString name, QIcon icon);
    QTreeWidgetItem* addResourceFolder(QTreeWidgetItem *node, QString name);
    void addResourceItem(QTreeWidgetItem *node, QString name, QIcon icon);

    void readSettings();
    void writeSettings();

    void loadFile(const QString &fileName);
    void saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    void updateRecentFileActions();

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];

};

#endif // MAINWINDOW_H
