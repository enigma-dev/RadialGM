/**
* @file  gameinfoeditor.cpp
* @brief Source implementing a class to create a game information rich text editor.
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

#include "gameinfoeditor.h"

GameInfoEditor::GameInfoEditor(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowTitle("Game Information");
    QMenuBar* menuBar = new QMenuBar();
    QMenu* fileMenu = new QMenu("&File");
    menuBar->addMenu(fileMenu);
    QMenu* editMenu = new QMenu("&Edit");
    menuBar->addMenu(editMenu);
    QMenu* formatMenu = new QMenu("&Format");
    menuBar->addMenu(formatMenu);
    this->setMenuBar(menuBar);

    textEditor = new QTextEdit();
    this->setCentralWidget(textEditor);

    QToolBar* editToolbar = new QToolBar();
    editToolbar->addAction(QIcon(":/icons/actions/accept.png"), "Save");
    editToolbar->addSeparator();
    QAction* saveAction = new QAction(QIcon(":/icons/actions/save.png"), "Save to file", NULL);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
    editToolbar->addAction(saveAction);
    QAction* openAction = new QAction(QIcon(":/icons/actions/open.png"), "Open from file", NULL);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
    editToolbar->addAction(openAction);
    //editToolbar->addAction(QIcon(":/icons/actions/open.png"), "Load from file");
    editToolbar->addAction(QIcon(":/icons/actions/print.png"), "Print");
    editToolbar->addSeparator();

    QAction* undoAction = new QAction(QIcon(":/icons/actions/undo.png"), "Undo", NULL);
    connect(undoAction, SIGNAL(triggered()), textEditor, SLOT(undo()));
    editToolbar->addAction(undoAction);
    QAction* redoAction = new QAction(QIcon(":/icons/actions/redo.png"), "Redo", NULL);
    connect(redoAction, SIGNAL(triggered()), textEditor, SLOT(redo()));
    editToolbar->addAction(redoAction);
    editToolbar->addSeparator();

    QAction* cutAction = new QAction(QIcon(":/icons/actions/cut.png"), "Cut", NULL);
    connect(cutAction, SIGNAL(triggered()), textEditor, SLOT(cut()));
    editToolbar->addAction(cutAction);
    QAction* copyAction = new QAction(QIcon(":/icons/actions/copy.png"), "Copy", NULL);
    connect(copyAction, SIGNAL(triggered()), textEditor, SLOT(copy()));
    editToolbar->addAction(copyAction);
    QAction* pasteAction = new QAction(QIcon(":/icons/actions/paste.png"), "Paste", NULL);
    connect(pasteAction, SIGNAL(triggered()), textEditor, SLOT(paste()));
    editToolbar->addAction(pasteAction);
    //editToolbar->addSeparator();


    this->addToolBar(editToolbar);
}

GameInfoEditor::~GameInfoEditor()
{

}

void GameInfoEditor::openFile() {
    QString fname = QFileDialog::getOpenFileName(this);
    if (fname.isEmpty()) { return; }
    QFile file(fname);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file for reading"));
        return;
    }

    QTextStream readStream(&file);
    textEditor->setHtml(readStream.readAll());

    file.close();
}

void GameInfoEditor::saveFile() {
    QString fname = QFileDialog::getSaveFileName(this);
    if (fname.isEmpty()) { return; }
    QFile file(fname);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file for writing"));
        return;
    }

    QTextStream writeStream(&file);
    writeStream << textEditor->document();

    file.close();
}
