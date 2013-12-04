/**
* @file  scriptwidget.cpp
* @brief Header implementing a class to create a script editor.
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

#include "scriptwidget.h"

ScriptWidget::ScriptWidget(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowTitle("Script");
    this->setWindowIcon(QIcon(":/resources/icons/resources/script.png"));
    sciEditor = new QsciScintilla(this);

    sciEditor->setFrameStyle(QsciScintilla::NoFrame);
    //sciEditor->setWrapMode(QsciScintilla::WrapCharacter);

    sciEditor->setCaretLineVisible(true);
    sciEditor->setCaretLineBackgroundColor(QColor("#ffe4e4"));

    QFont font = QFont("Courier", 8);
    font.setFixedPitch(true);
    //sciEditor->setFont(font);
    QsciLexerCPP lexer;
    lexer.setFont(font);
    lexer.setFoldCompact(false);
    sciEditor->setLexer(&lexer);
    QFontMetrics fontmetrics = QFontMetrics(font);
    sciEditor->setMarginWidth(0, fontmetrics.width("__")+8);
    sciEditor->setMarginLineNumbers(0, true);
    //sciEditor->setMarginsBackgroundColor(QColor("#dddddd"));

    sciEditor->setMarginSensitivity(1, true);
    BREAK_MARKER_NUM = 8;
    //this->setMarginWidth();
    connect(sciEditor,
        SIGNAL(marginClicked(int, int, Qt::KeyboardModifiers)), this,
                       SLOT(on_margin_clicked(int, int, Qt::KeyboardModifiers)));
    sciEditor->markerDefine(QImage(":/icons/actions/link_break.png"),
        BREAK_MARKER_NUM);
    sciEditor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    sciEditor->setWhitespaceVisibility(QsciScintilla::WsInvisible);

    sciEditor->setFolding(QsciScintilla::BoxedTreeFoldStyle, 3);
    //this->setFoldMarginColors(QColor("#dddddd"), QColor("#dddddd"));

    sciEditor->setMarginsFont(font);
    sciEditor->setMarginsForegroundColor(QColor("#bbbbbb"));

    QVBoxLayout *layout = new QVBoxLayout(this); // no initialization here
    editToolbar = new QToolBar(this);
    editToolbar->setFloatable(true);
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
    connect(undoAction, SIGNAL(triggered()), sciEditor, SLOT(undo()));
    editToolbar->addAction(undoAction);
    QAction* redoAction = new QAction(QIcon(":/icons/actions/redo.png"), "Redo", NULL);
    connect(redoAction, SIGNAL(triggered()), sciEditor, SLOT(redo()));
    editToolbar->addAction(redoAction);
    editToolbar->addSeparator();

    QAction* cutAction = new QAction(QIcon(":/icons/actions/cut.png"), "Cut", NULL);
    connect(cutAction, SIGNAL(triggered()), sciEditor, SLOT(cut()));
    editToolbar->addAction(cutAction);
    QAction* copyAction = new QAction(QIcon(":/icons/actions/copy.png"), "Copy", NULL);
    connect(copyAction, SIGNAL(triggered()), sciEditor, SLOT(copy()));
    editToolbar->addAction(copyAction);
    QAction* pasteAction = new QAction(QIcon(":/icons/actions/paste.png"), "Paste", NULL);
    connect(pasteAction, SIGNAL(triggered()), sciEditor, SLOT(paste()));
    editToolbar->addAction(pasteAction);
    editToolbar->addSeparator();

    editToolbar->addAction(QIcon(":/icons/actions/find.png"), "Find and Replace");
    editToolbar->addAction(QIcon(":/icons/actions/line-goto.png"), "Go to line");
    editToolbar->addSeparator();
    editToolbar->addWidget(new QLabel("Name:"));
    editToolbar->setStyleSheet(" QToolBar { height: 18px; width: 18px; icon-size: 18px; } ");
    QLineEdit* nameEdit = new QLineEdit(this);
    nameEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    editToolbar->addWidget(nameEdit);
    layout->addWidget(editToolbar);
    layout->addWidget(sciEditor); // layout is uninitialized and probably garbage

    QStatusBar* statusBar = new QStatusBar();
    statusBar->showMessage("INS | UTF-8 | 1:1 | No Errors");
    layout->addWidget(statusBar);

    layout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(layout);

   // sciEditor->setFixedHeight(300);
}

ScriptWidget::~ScriptWidget()
{

}

void ScriptWidget::on_margin_clicked(int nmargin, int nline, Qt::KeyboardModifiers modifiers) {
    // Toggle marker for the line the margin was clicked on
    if (sciEditor->markersAtLine(nline) != 0) {
        sciEditor->markerDelete(nline, BREAK_MARKER_NUM);
    } else {
        sciEditor->markerAdd(nline, BREAK_MARKER_NUM);
    }
}

void ScriptWidget::openFile() {
    QString fname = QFileDialog::getOpenFileName(this);
    if (fname.isEmpty()) { return; }
    QFile file(fname);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file for reading"));
        return;
    }

    QTextStream readStream(&file);
    sciEditor->setText(readStream.readAll());

    file.close();
}

void ScriptWidget::saveFile() {
    QString fname = QFileDialog::getSaveFileName(this);
    if (fname.isEmpty()) { return; }
    QFile file(fname);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file for writing"));
        return;
    }

    QTextStream writeStream(&file);
    writeStream << sciEditor->text();

    file.close();
}
