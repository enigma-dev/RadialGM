/**
* @file  shaderwidget.cpp
* @brief Header implementing a class to create a shader editor.
*
* @section License
*
* Copyright (C) 2013 Robert B. Colton
* This file is a part of the RadialGM IDE.
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

#include "shaderwidget.h"

ShaderWidget::ShaderWidget(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowTitle("Shader");
    this->setWindowIcon(QIcon(":/resources/icons/resources/shader.png"));

    vsciEditor = createEditor();
    fsciEditor = createEditor();

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
    editToolbar->addAction(QIcon(":/icons/actions/print.png"), "Print");
    editToolbar->addSeparator();
    QAction* undoAction = new QAction(QIcon(":/icons/actions/undo.png"), "Undo", NULL);
    connect(undoAction, SIGNAL(triggered()), this, SLOT(undo()));
    editToolbar->addAction(undoAction);
    QAction* redoAction = new QAction(QIcon(":/icons/actions/redo.png"), "Redo", NULL);
    connect(redoAction, SIGNAL(triggered()), this, SLOT(redo()));
    editToolbar->addAction(redoAction);
    editToolbar->addSeparator();

    QAction* cutAction = new QAction(QIcon(":/icons/actions/cut.png"), "Cut", NULL);
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));
    editToolbar->addAction(cutAction);
    QAction* copyAction = new QAction(QIcon(":/icons/actions/copy.png"), "Copy", NULL);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    editToolbar->addAction(copyAction);
    QAction* pasteAction = new QAction(QIcon(":/icons/actions/paste.png"), "Paste", NULL);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
    editToolbar->addAction(pasteAction);
    editToolbar->addSeparator();
    editToolbar->addAction(QIcon(":/icons/actions/find.png"), "Find and Replace");
    editToolbar->addAction(QIcon(":/icons/actions/line-goto.png"), "Go to line");
    editToolbar->addSeparator();
    editToolbar->addWidget(new QLabel("Name:"));
    QLineEdit* nameEdit = new QLineEdit(this);
    nameEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    editToolbar->addWidget(nameEdit);
    editToolbar->addSeparator();
    editToolbar->addWidget(new QLabel("Type:"));
    QComboBox* typeCombo = new QComboBox();
    typeCombo->addItem("GLSLES");
    typeCombo->addItem("GLSL");
    typeCombo->addItem("HLSL11");
    typeCombo->addItem("HLSL9");
    editToolbar->addWidget(typeCombo);
    editToolbar->setStyleSheet(" QToolBar { height: 18px; width: 18px; icon-size: 18px; } ");
    layout->addWidget(editToolbar);
    mainTabWidget = new QTabWidget();
    mainTabWidget->addTab(vsciEditor, "Vertex");
    mainTabWidget->addTab(fsciEditor, "Fragment");
    layout->addWidget(mainTabWidget); // layout is uninitialized and probably garbage

    QStatusBar* statusBar = new QStatusBar();
    statusBar->showMessage("INS | UTF-8 | 1:1");
    layout->addWidget(statusBar);

    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

   // sciEditor->setFixedHeight(300);
}

ShaderWidget::~ShaderWidget()
{

}

QsciScintilla* ShaderWidget::createEditor() {
    QsciScintilla* editor = new QsciScintilla(this);

    editor->setFrameStyle(QsciScintilla::NoFrame);
    //editor->setWrapMode(QsciScintilla::WrapCharacter);

    editor->setCaretLineVisible(true);
    editor->setCaretLineBackgroundColor(QColor("#ffe4e4"));

    QFont font = QFont("Courier 10 Pitch", 10);
    font.setFixedPitch(true);
    editor->setFont(font);
    QsciLexerCPP lexer;
    lexer.setFont(font);
    editor->setLexer(&lexer);
    QFontMetrics fontmetrics = QFontMetrics(font);
    editor->setMarginWidth(0, fontmetrics.width("__")+8);
    editor->setMarginLineNumbers(0, true);
    //sciEditor->setMarginsBackgroundColor(QColor("#dddddd"));

    editor->setMarginSensitivity(1, true);
    BREAK_MARKER_NUM = 8;
    //this->setMarginWidth();
    connect(editor,
        SIGNAL(marginClicked(int, int, Qt::KeyboardModifiers)), this,
                       SLOT(marginClicked(int, int, Qt::KeyboardModifiers)));
    editor->markerDefine(QImage(":/icons/actions/link_break.png"),
        BREAK_MARKER_NUM);
    editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);

    editor->setFolding(QsciScintilla::BoxedTreeFoldStyle, 3);
    //this->setFoldMarginColors(QColor("#dddddd"), QColor("#dddddd"));

    editor->setMarginsFont(font);
    editor->setMarginsForegroundColor(QColor("#bbbbbb"));

    return editor;
}

void ShaderWidget::marginClicked(int nmargin, int nline, Qt::KeyboardModifiers modifiers) {
    QsciScintilla* widget = reinterpret_cast<QsciScintilla *>( mainTabWidget->currentWidget() );
    if (!widget) { return; }

    // Toggle marker for the line the margin was clicked on
    if (widget->markersAtLine(nline) != 0) {
        widget->markerDelete(nline, BREAK_MARKER_NUM);
    } else {
        widget->markerAdd(nline, BREAK_MARKER_NUM);
    }
}

void ShaderWidget::openFile() {
    QsciScintilla* widget = reinterpret_cast<QsciScintilla *>( mainTabWidget->currentWidget() );
    if (!widget) { return; }

    QString fname = QFileDialog::getOpenFileName(this);
    if (fname.isEmpty()) { return; }
    QFile file(fname);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file for reading"));
        return;
    }

    QTextStream readStream(&file);
    widget->setText(readStream.readAll());

    file.close();
}

void ShaderWidget::saveFile() {
    QsciScintilla* widget = reinterpret_cast<QsciScintilla *>( mainTabWidget->currentWidget() );
    if (!widget) { return; }

    QString fname = QFileDialog::getSaveFileName(this);
    if (fname.isEmpty()) { return; }
    QFile file(fname);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file for writing"));
        return;
    }

    QTextStream writeStream(&file);
    writeStream << widget->text();

    file.close();
}

void ShaderWidget::undo() {
    QsciScintilla* widget = reinterpret_cast<QsciScintilla *>( mainTabWidget->currentWidget() );
    if (!widget) { return; }
    widget->undo();
}

void ShaderWidget::redo() {
    QsciScintilla* widget = reinterpret_cast<QsciScintilla *>( mainTabWidget->currentWidget() );
    if (!widget) { return; }
    widget->redo();
}

void ShaderWidget::cut() {
    QsciScintilla* widget = reinterpret_cast<QsciScintilla *>( mainTabWidget->currentWidget() );
    if (!widget) { return; }
    widget->cut();
}

void ShaderWidget::copy() {
    QsciScintilla* widget = reinterpret_cast<QsciScintilla *>( mainTabWidget->currentWidget() );
    if (!widget) { return; }
    widget->copy();
}

void ShaderWidget::paste() {
    QsciScintilla* widget = reinterpret_cast<QsciScintilla *>( mainTabWidget->currentWidget() );
    if (!widget) { return; }
    widget->paste();
}
