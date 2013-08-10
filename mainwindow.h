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
#include "customwidgets/qttoolbar.h"
#include "widgets/scriptwidget.h"
#include "widgets/fontwidget.h"
#include "widgets/pathwidget.h"
#include "widgets/objectwidget.h"
#include "widgets/timelinewidget.h"
#include "widgets/backgroundwidget.h"
#include "widgets/soundwidget.h"
#include "widgets/roomwidget.h"
#include "widgets/spritewidget.h"
#include "widgets/modelwidget.h"
#include "widgets/materialwidget.h"
#include "dialogs/aboutdialog.h"

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

private:

    QtToolBar* fileToolbar;
    QAction* newAction;
    QAction* openAction;
    QAction* saveAction;
    QAction* saveAsAction;
    QtToolBar* buildToolbar;
    QAction* runAction;
    QAction* debugAction;
    QAction* compileAction;
    QtToolBar* resourceToolbar;
    QAction* spriteAction;
    QAction* soundAction;
    QAction* backgroundAction;
    QAction* pathAction;
    QAction* scriptAction;
    QAction* fontAction;
    QAction* timelineAction;
    QAction* objectAction;
    QAction* roomAction;
    QtToolBar* settingsToolbar;
    QAction* preferencesAction;
    QAction* gameSettingsAction;
    QAction* gameInformationAction;
    QAction* extensionsAction;
    QAction* manualAction;

    AboutDialog* aboutDialog;

    QStatusBar* mainStatusBar;
    QProgressBar* mainProgressBar;
    QMenuBar* mainMenuBar;
    QMenu* fileMenu;
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

    void addResource(QString name, QIcon icon);
    void addResourceGroup(QString name);

public slots:
    void closeApplication();
    void showLicenseDialog();
    void showAboutDialog();
    void toggleMdiTabs();
};

#endif // MAINWINDOW_H
