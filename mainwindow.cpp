/**
* @file  mainwindow.cpp
* @brief Source implementing a class for the main window.
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

#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QAction* clearRecentFilesAction = new QAction("Clear", this);
    connect(clearRecentFilesAction, SIGNAL(triggered()), this, SLOT(clearRecentFiles()));
    QAction* exitAction = new QAction("&Exit", this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(closeApplication()));
    QAction* mdiAction = new QAction("&Multiple Document Interface", this);
    connect(mdiAction, SIGNAL(triggered()), this, SLOT(toggleMdiTabs()));
    QAction* hierarchyAction = new QAction("&Hierarchy", this);
    connect(hierarchyAction, SIGNAL(triggered()), this, SLOT(toggleHierarchy()));
    QAction* logAction = new QAction("&Output Log", this);
    connect(logAction, SIGNAL(triggered()), this, SLOT(toggleOutputLog()));
    QAction* msgsAction = new QAction("&Output Messages", this);
    connect(msgsAction, SIGNAL(triggered()), this, SLOT(toggleOutputMessages()));
    QAction* restoreAction = new QAction("&Restore Layout", this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(restoreLayout()));
    QAction* cascadeAction = new QAction(QIcon(":/icons/actions/cascade.png"), "&Cascade", this);
    connect(cascadeAction, SIGNAL(triggered()), this, SLOT(cascadeWindows()));
    QAction* tileAction = new QAction("&Tile", this);
    connect(tileAction, SIGNAL(triggered()), this, SLOT(tileWindows()));
    QAction* closeAction = new QAction("&Close", this);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(closeWindow()));
    QAction* closeAllAction = new QAction("&Close All", this);
    connect(closeAllAction, SIGNAL(triggered()), this, SLOT(closeAllWindows()));
    QAction* licenseAction = new QAction("&License", this);
    connect(licenseAction, SIGNAL(triggered()), this, SLOT(showLicenseDialog()));
    QAction* aboutAction = new QAction("&About", this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
    preferencesAction = new QAction(QIcon(":/icons/actions/preferences.png"), "Preferences", this);
    connect(preferencesAction, SIGNAL(triggered()), this, SLOT(showPreferencesDialog()));
    manualAction = new QAction(QIcon(":/icons/actions/manual.png"), "Documentation", this);
    connect(manualAction, SIGNAL(triggered()), this, SLOT(showManual()));

    mainMenuBar = new QMenuBar(this);
    fileMenu = new QMenu("&File", this);
    QMenu* viewMenu = new QMenu("&View", this);
    viewMenu->addAction(mdiAction);
    viewMenu->addSeparator();
    viewMenu->addAction(hierarchyAction);
    viewMenu->addAction(logAction);
    viewMenu->addAction(msgsAction);
    viewMenu->addSeparator();
    viewMenu->addAction(restoreAction);
    editMenu = new QMenu("&Edit", this);
    resourceMenu = new QMenu("&Resources", this);
    windowMenu = new QMenu("&Window", this);
    windowMenu->addAction(cascadeAction);
    windowMenu->addAction(tileAction);
    windowMenu->addAction(closeAction);
    windowMenu->addAction(closeAllAction);
    helpMenu = new QMenu("&Help", this);
    helpMenu->addAction(licenseAction);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(manualAction);
    mainMenuBar->addMenu(fileMenu);
    mainMenuBar->addMenu(viewMenu);
    mainMenuBar->addMenu(editMenu);
    mainMenuBar->addMenu(resourceMenu);
    mainMenuBar->addMenu(windowMenu);
    mainMenuBar->addMenu(helpMenu);
    this->setMenuBar(mainMenuBar);

    fileToolbar = new QToolBar("File");
    fileToolbar->setObjectName("fileToolbar");
    newAction = new QAction(QIcon(":/icons/actions/new.png"), "New", this);
    fileToolbar->addAction(newAction);
    fileMenu->addAction(newAction);
    openAction = new QAction(QIcon(":/icons/actions/open.png"), "Open", this);
    connect(openAction, SIGNAL(triggered()), this, SLOT(showOpenDialog()));
    fileToolbar->addAction(openAction);
    fileMenu->addAction(openAction);

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(cut()));
    }
    recentFilesMenu = new QMenu("&Recent Files");
    for (int i = 0; i < MaxRecentFiles; ++i)
        recentFilesMenu->addAction(recentFileActs[i]);
    recentFilesMenu->addSeparator();
    recentFilesMenu->addAction(clearRecentFilesAction);
    fileMenu->addMenu(recentFilesMenu);
    this->updateRecentFileActions();

    saveAction = new QAction(QIcon(":/icons/actions/save.png"), "Save", this);
    fileToolbar->addAction(saveAction);
    fileMenu->addAction(saveAction);
    saveAsAction = new QAction(QIcon(":/icons/actions/save-as.png"), "Save As", this);

    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(showSaveDialog()));
    fileToolbar->addAction(saveAsAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(preferencesAction);
    fileMenu->addSeparator();
    // add recent projects list
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    this->addToolBar(fileToolbar);
    fileToolbar->setStyleSheet(" QToolButton { height: 18px; width: 18px; icon-size: 18px; } ");

    buildToolbar = new QToolBar("Build");
    buildToolbar->setObjectName("buildToolbar");
    runAction = new QAction(QIcon(":/icons/actions/execute.png"), "Execute", this);
    buildToolbar->addAction(runAction);
    debugAction = new QAction(QIcon(":/icons/actions/debug.png"), "Debug", this);
    buildToolbar->addAction(debugAction);
    compileAction = new QAction(QIcon(":/icons/actions/compile.png"), "Compile", this);
    buildToolbar->addAction(compileAction);
    this->addToolBar(buildToolbar);
    buildToolbar->setStyleSheet(" QToolButton { height: 18px; width: 18px; icon-size: 18px; } ");

    resourceToolbar = new QToolBar("Resource");
    resourceToolbar->setObjectName("resourceToolbar");
    spriteAction = new QAction(QIcon(":/resources/icons/resources/sprite.png"), "New Sprite", this);
    connect(spriteAction, SIGNAL(triggered()), this, SLOT(addSprite()));
    resourceToolbar->addAction(spriteAction);
    soundAction = new QAction(QIcon(":/resources/icons/resources/sound.png"), "New Sound", this);
    connect(soundAction, SIGNAL(triggered()), this, SLOT(addSound()));
    resourceToolbar->addAction(soundAction);
    backgroundAction = new QAction(QIcon(":/resources/icons/resources/background.png"), "New Background", this);
    connect(backgroundAction, SIGNAL(triggered()), this, SLOT(addBackground()));
    resourceToolbar->addAction(backgroundAction);
    pathAction = new QAction(QIcon(":/resources/icons/resources/path.png"), "New Path", this);
    connect(pathAction, SIGNAL(triggered()), this, SLOT(addPath()));
    resourceToolbar->addAction(pathAction);
    scriptAction = new QAction(QIcon(":/resources/icons/resources/script.png"), "New Script", this);
    connect(scriptAction, SIGNAL(triggered()), this, SLOT(addScript()));
    resourceToolbar->addAction(scriptAction);
    QAction* shaderAction = new QAction(QIcon(":/resources/icons/resources/shader.png"), "New Shader", this);
    connect(shaderAction, SIGNAL(triggered()), this, SLOT(addShader()));
    resourceToolbar->addAction(shaderAction);
    fontAction = new QAction(QIcon(":/resources/icons/resources/font.png"), "New Font", this);
    connect(fontAction, SIGNAL(triggered()), this, SLOT(addFont()));
    resourceToolbar->addAction(fontAction);
    timelineAction = new QAction(QIcon(":/resources/icons/resources/timeline.png"), "New Timeline", this);
    connect(timelineAction, SIGNAL(triggered()), this, SLOT(addTimeline()));
    resourceToolbar->addAction(timelineAction);
    objectAction = new QAction(QIcon(":/resources/icons/resources/object.png"), "New Object", this);
    connect(objectAction, SIGNAL(triggered()), this, SLOT(addObject()));
    resourceToolbar->addAction(objectAction);
    roomAction = new QAction(QIcon(":/resources/icons/resources/room.png"), "New Room", this);
    connect(roomAction, SIGNAL(triggered()), this, SLOT(addRoom()));
    resourceToolbar->addAction(roomAction);
    this->addToolBar(resourceToolbar);
    resourceToolbar->setStyleSheet(" QToolButton { height: 18px; width: 18px; icon-size: 18px; } ");

    settingsToolbar = new QToolBar("Settings");
    settingsToolbar->setObjectName("settingsToolbar");
    settingsToolbar->addAction(preferencesAction);
    gameSettingsAction = new QAction(QIcon(":/resources/icons/resources/gm.png"), "Global Game Settings", this);
    settingsToolbar->addAction(gameSettingsAction);
    gameInformationAction = new QAction(QIcon(":/resources/icons/resources/info.png"), "Game Information", this);
    connect(gameInformationAction, SIGNAL(triggered()), this, SLOT(showGameInformation()));
    settingsToolbar->addAction(gameInformationAction);
    extensionsAction = new QAction(QIcon(":/resources/icons/resources/extension.png"), "Extensions", this);
    settingsToolbar->addAction(extensionsAction);
    settingsToolbar->addAction(manualAction);
    this->addToolBar(settingsToolbar);
    settingsToolbar->setStyleSheet(" QToolButton { height: 18px; width: 18px; icon-size: 18px; } ");

    hierarchyDock = new QDockWidget("Hierarchy", this, Qt::WindowTitleHint);
    hierarchyDock->setObjectName("hierarchyDock");
    hierarchyTree = new QTreeWidget(this);
    hierarchyTree->header()->setVisible(false);
    spriteGroup = addResourceGroup("Sprites");
    soundGroup = addResourceGroup("Sounds");
    backgroundGroup = addResourceGroup("Backgrounds");
    pathGroup = addResourceGroup("Paths");
    scriptGroup = addResourceGroup("Scripts");
    shaderGroup = addResourceGroup("Shaders");
    fontGroup = addResourceGroup("Fonts");
    timelineGroup = addResourceGroup("Timelines");
    objectGroup = addResourceGroup("Objects");
    roomGroup = addResourceGroup("Rooms");

    //Example Populations, can reference folders via pointers or resourceItemMap
    addResourceItem(objectGroup, "Object Ex1", QIcon(":/resources/icons/resources/info.png"));
    QTreeWidgetItem* folder1 = addResourceFolder(objectGroup, "Folder 1");
    addResourceItem(resourceItemMap[objectGroup]->at(0), "Object Ex2", QIcon(":/resources/icons/resources/info.png"));
    QTreeWidgetItem* folder2 = addResourceFolder(folder1, "Folder 2");
    addResourceItem(resourceItemMap[folder1]->at(0), "Object Ex3", QIcon(":/resources/icons/resources/info.png"));
    addResourceItem(folder2, "Object Ex4", QIcon(":/resources/icons/resources/info.png"));

    addResource("Game Information", QIcon(":/resources/icons/resources/info.png"));
    addResource("Global Game Settings", QIcon(":/resources/icons/resources/gm.png"));
    addResource("Extensions", QIcon(":/resources/icons/resources/extension.png"));
    hierarchyTree->setIconSize(QSize(18, 18));
    hierarchyDock->setWidget(hierarchyTree);

    this->addDockWidget(Qt::LeftDockWidgetArea, hierarchyDock);

    setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
    setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
    setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
    setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );

    logDock = new QDockWidget("Log", this, Qt::WindowTitleHint);
    logDock->setObjectName("logDock");
    logText = new QTextEdit(this);
    logText->setReadOnly(true);
    logDock->setWidget(logText);
    messagesDock = new QDockWidget("Messages", this, Qt::WindowTitleHint);
    messagesDock->setObjectName("messagesDock");
    messagesTable = new QTableWidget(this);
    messagesTable->setColumnCount(4);
    messagesTable->setRowCount(0);
    QStringList headers;
    headers.append("#");
    headers.append("Origin");
    headers.append("Location");
    headers.append("Description");
    messagesTable->setHorizontalHeaderLabels(headers);
    messagesTable->setSortingEnabled(true);

    messagesDock->setWidget(messagesTable);
    this->addDockWidget(Qt::BottomDockWidgetArea, messagesDock);
    this->addDockWidget(Qt::BottomDockWidgetArea, logDock);
    tabifyDockWidget(logDock, messagesDock);

    mainMdiArea = new QMdiArea(this);
    mainMdiArea->setBackground(QBrush(QPixmap("lgm1.png")));
    mainMdiArea->setTabsClosable(true);
    mainMdiArea->setDocumentMode(true);
    this->setCentralWidget(mainMdiArea);

    mainStatusBar = new QStatusBar(this);
    this->setStatusBar(mainStatusBar);
    mainStatusBar->showMessage("Ready");
    //mainProgressBar = new QProgressBar();
    //mainProgressBar->setValue(75);
    //mainStatusBar->addWidget(mainProgressBar);

    this->setWindowIcon(QIcon(":/lgm-logo.png"));
    this->setWindowTitle("LateralGM - <new game>");
    this->resize(1000, 600);

    outputLine("test");
    outputLine("test2");
    outputMessage("obj_player", "Create Event", "Lorem ipsum dollar sit amit...");
    outputMessage("obj_car", "Name", "Resource name cannot contain spaces.");
    outputMessage("obj_gun", "Draw Event", "Lorem ipsum dollar sit amit...");

    aboutDialog = NULL;
    prefsDialog = NULL;
    infoEditor = NULL;

    this->setObjectName("mainWindow");
    defaultState = this->saveState();
    this->readSettings();
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeApplication() {
    this->writeSettings();
    this->close();
}

void MainWindow::closeEvent(QCloseEvent *event) {

}

void MainWindow::showLicenseDialog() {
    if (aboutDialog == NULL) {
        aboutDialog = new AboutDialog(this);
    }
    aboutDialog->show(":/license.html", "License");
}

void MainWindow::showAboutDialog() {
    if (aboutDialog == NULL) {
        aboutDialog = new AboutDialog(this);
    }
    aboutDialog->show(":/about.html", "About");
}

void MainWindow::showPreferencesDialog() {
    if (prefsDialog == NULL) {
        prefsDialog = new PreferencesDialog(this);
    }
    prefsDialog->show();
}

void MainWindow::showGameInformation() {
    if (infoEditor == NULL) {
        infoEditor = new GameInfoEditor(this);
    }
    // QMainWindow can be added as a subwindow
    //this->mainMdiArea->addSubWindow(infoEditor,Qt::SubWindow);
    infoEditor->show();
}


void MainWindow::showOpenDialog() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Project"), "", tr("All Files (*.*);;GMK Files (*.gmk)"));
    currentFile = new ProjectManager();
    currentFile->LoadGMK(fileName);

    QSettings settings("LateralGMTeam", "LateralGM");
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    this->updateRecentFileActions();
}

void MainWindow::showSaveDialog() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Project"), "", tr("All Files (*.*);;GMK Files (*.gmk)"));
}

void MainWindow::showManual() {
    QDesktopServices::openUrl(QUrl("http://enigma-dev.org/docs/Wiki/Main_Page"));
}

void MainWindow::toggleMdiTabs() {
    mainMdiArea->setDocumentMode(true);
    switch (mainMdiArea->viewMode()) {
    case QMdiArea::TabbedView:
         mainMdiArea->setViewMode(QMdiArea::SubWindowView);
        break;
    case QMdiArea::SubWindowView:
         mainMdiArea->setViewMode(QMdiArea::TabbedView);
        break;
    }
}

void MainWindow::toggleHierarchy() {
    if (hierarchyDock->isVisible()) {
        hierarchyDock->hide();
    } else {
        hierarchyDock->show();
    }

}

void MainWindow::toggleOutputLog() {
    if (logDock->isVisible()) {
        logDock->hide();
    } else {
        logDock->show();
    }
}

void MainWindow::toggleOutputMessages() {
    if (messagesDock->isVisible()) {
        messagesDock->hide();
    } else {
        messagesDock->show();
    }
}

void MainWindow::addSprite() {
    SpriteWidget* editorWidget = new SpriteWidget();
    mainMdiArea->addSubWindow(editorWidget, Qt::Window);
    editorWidget->show();
}

void MainWindow::addBackground() {
    BackgroundWidget* editorWidget = new BackgroundWidget();
    mainMdiArea->addSubWindow(editorWidget, Qt::Window);
    editorWidget->show();
}

void MainWindow::addSound() {
    SoundWidget* editorWidget = new SoundWidget();
    mainMdiArea->addSubWindow(editorWidget, Qt::Window);
    editorWidget->show();
}

void MainWindow::addFont() {
    FontWidget* editorWidget = new FontWidget();
    mainMdiArea->addSubWindow(editorWidget, Qt::Window);
    editorWidget->show();
}

void MainWindow::addShader() {
    ShaderWidget* editorWidget = new ShaderWidget();
    mainMdiArea->addSubWindow(editorWidget, Qt::Window);
    editorWidget->show();
}

void MainWindow::addTimeline() {
    TimelineWidget* editorWidget = new TimelineWidget();
    mainMdiArea->addSubWindow(editorWidget, Qt::Window);
    editorWidget->show();
}

void MainWindow::addObject() {
    ObjectWidget* editorWidget = new ObjectWidget();
    mainMdiArea->addSubWindow(editorWidget, Qt::Window);
    editorWidget->show();
}

void MainWindow::addRoom() {
    RoomWidget* editorWidget = new RoomWidget();
    mainMdiArea->addSubWindow(editorWidget, Qt::Window);
    editorWidget->show();
}

void MainWindow::addPath() {
    PathWidget* editorWidget = new PathWidget();
    mainMdiArea->addSubWindow(editorWidget, Qt::Window);
    editorWidget->show();
}

void MainWindow::addScript() {
    ScriptWidget* editorWidget = new ScriptWidget();
    mainMdiArea->addSubWindow(editorWidget, Qt::Window);
    editorWidget->show();
}

void MainWindow::closeAllWindows() {
    mainMdiArea->closeAllSubWindows();
}

void MainWindow::closeWindow() {
    mainMdiArea->closeActiveSubWindow();
}

void MainWindow::restoreLayout() {
    this->restoreState(defaultState);
}

void MainWindow::cascadeWindows() {
    mainMdiArea->cascadeSubWindows();
}

void MainWindow::tileWindows() {
    mainMdiArea->tileSubWindows();
}

void MainWindow::outputClear(bool clearLog, bool clearMessages)
{
    if (clearLog) {
        this->logText->clear();
    }
    if (clearMessages) {
        this->messagesTable->clearContents();
        this->messagesTable->setRowCount(0);
    }
}

void MainWindow::outputText(QString text)
{
    this->logText->insertPlainText(text);
}

void MainWindow::outputLine(QString text)
{
    this->logText->append(text);
}

void MainWindow::outputMessage(QString origin, QString location, QString description)
{
    int ind = messagesTable->rowCount();
    messagesTable->insertRow(ind);
    messagesTable->setItem(ind, 0, new QTableWidgetItem(QString::number(ind)));
    messagesTable->setItem(ind, 1, new QTableWidgetItem(origin));
    messagesTable->setItem(ind, 2, new QTableWidgetItem(location));
    messagesTable->setItem(ind, 3, new QTableWidgetItem(description));
}

QTreeWidgetItem* MainWindow::addResourceGroup(QString name)
{
    QTreeWidgetItem* treeItem = new QTreeWidgetItem();
    treeItem->setText(0, name);
    treeItem->setIcon(0, QIcon(":/resources/icons/resources/group.png"));

    QFont fnt = treeItem->font(0);
    fnt.setBold(true);
    treeItem->setFont(0, fnt);
    hierarchyTree->addTopLevelItem(treeItem);
    resourceItemMap[treeItem] = new QList<QTreeWidgetItem*>;
    return treeItem;
}

void MainWindow::addResource(QString name, QIcon icon)
{
    QTreeWidgetItem* treeItem = new QTreeWidgetItem();
    treeItem->setText(0, name);
    treeItem->setIcon(0, icon);
    hierarchyTree->addTopLevelItem(treeItem);
}

QTreeWidgetItem* MainWindow::addResourceFolder(QTreeWidgetItem *node, QString name)
{
    QTreeWidgetItem* treeItem = new QTreeWidgetItem(node);
    treeItem->setText(0, name);
    treeItem->setIcon(0, QIcon(":/resources/icons/resources/group.png"));

    QFont fnt = treeItem->font(0);
    treeItem->setFont(0, fnt);
    resourceItemMap[node]->append(treeItem);
    resourceItemMap[treeItem] = new QList<QTreeWidgetItem*>;
    return treeItem;
}

void MainWindow::addResourceItem(QTreeWidgetItem *node, QString name, QIcon icon)
{
    QTreeWidgetItem* treeItem = new QTreeWidgetItem(node);
    treeItem->setText(0, name);
    treeItem->setIcon(0, icon);
}

void MainWindow::readSettings()
{
    QSettings settings("LateralGMTeam", "LateralGM");
    restoreGeometry(settings.value("mainWindow/geometry").toByteArray());
    restoreState(settings.value("mainWindowState").toByteArray());
}

void MainWindow::writeSettings()
{
    QSettings settings("LateralGMTeam", "LateralGM");
    settings.setValue("mainWindow/geometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings("LateralGMTeam", "LateralGM");
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    if (numRecentFiles == 0) {
        recentFilesMenu->setEnabled(false);
    } else {
        recentFilesMenu->setEnabled(true);
    }

    for (int i = 0; i < numRecentFiles; ++i) {
        //QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(files[i]);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);
}

void MainWindow::clearRecentFiles() {
    QSettings settings("LateralGMTeam", "LateralGM");
    settings.setValue("recentFileList", 0);

    this->updateRecentFileActions();
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
