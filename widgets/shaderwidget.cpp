/**
* @file  shaderwidget.cpp
* @brief Header implementing a class to create a shader editor.
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

#include "shaderwidget.h"

ShaderWidget::ShaderWidget(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowTitle("Script");
    this->setWindowIcon(QIcon(":/resources/icons/resources/script.png"));
    vsciEditor = new QsciScintilla(this);

    vsciEditor->setFrameStyle(QsciScintilla::NoFrame);
    //sciEditor->setWrapMode(QsciScintilla::WrapCharacter);

    vsciEditor->setCaretLineVisible(true);
    vsciEditor->setCaretLineBackgroundColor(QColor("#ffe4e4"));

    QFont font = QFont("Courier 10 Pitch", 10);
    font.setFixedPitch(true);
    vsciEditor->setFont(font);
    QsciLexerCPP lexer;
    lexer.setFont(font);
    vsciEditor->setLexer(&lexer);
    QFontMetrics fontmetrics = QFontMetrics(font);
    vsciEditor->setMarginWidth(0, fontmetrics.width("__")+8);
    vsciEditor->setMarginLineNumbers(0, true);
    //sciEditor->setMarginsBackgroundColor(QColor("#dddddd"));

    vsciEditor->setMarginSensitivity(1, true);
    BREAK_MARKER_NUM = 8;
    //this->setMarginWidth();
    connect(vsciEditor,
        SIGNAL(marginClicked(int, int, Qt::KeyboardModifiers)), this,
                       SLOT(on_vertex_margin_clicked(int, int, Qt::KeyboardModifiers)));
    vsciEditor->markerDefine(QImage(":/icons/actions/link_break.png"),
        BREAK_MARKER_NUM);
    vsciEditor->setBraceMatching(QsciScintilla::SloppyBraceMatch);

    vsciEditor->setFolding(QsciScintilla::BoxedTreeFoldStyle, 3);
    //this->setFoldMarginColors(QColor("#dddddd"), QColor("#dddddd"));

    vsciEditor->setMarginsFont(font);
    vsciEditor->setMarginsForegroundColor(QColor("#bbbbbb"));

    QVBoxLayout *layout = new QVBoxLayout(this); // no initialization here
    editToolbar = new QToolBar(this);
    editToolbar->setFloatable(true);
    editToolbar->addAction(QIcon(":/icons/actions/accept.png"), "Save");
    editToolbar->addSeparator();
    editToolbar->addAction(QIcon(":/icons/actions/save.png"), "Save to file");
    editToolbar->addAction(QIcon(":/icons/actions/open.png"), "Load from file");
    editToolbar->addAction(QIcon(":/icons/actions/print.png"), "Print");
    editToolbar->addSeparator();
    editToolbar->addAction(QIcon(":/icons/actions/undo.png"), "Undo");
    editToolbar->addAction(QIcon(":/icons/actions/redo.png"), "Redo");
    editToolbar->addSeparator();
    editToolbar->addAction(QIcon(":/icons/actions/cut.png"), "Cut");
    editToolbar->addAction(QIcon(":/icons/actions/copy.png"), "Copy");
    editToolbar->addAction(QIcon(":/icons/actions/paste.png"), "Paste");
    editToolbar->addSeparator();
    editToolbar->addAction(QIcon(":/icons/actions/find.png"), "Find and Replace");
    editToolbar->addAction(QIcon(":/icons/actions/line-goto.png"), "Go to line");
    editToolbar->addSeparator();
    editToolbar->addWidget(new QLabel("Name:"));
    QLineEdit* nameEdit = new QLineEdit(this);
    nameEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    editToolbar->addWidget(nameEdit);
    layout->addWidget(editToolbar);
    layout->addWidget(vsciEditor); // layout is uninitialized and probably garbage
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

   // sciEditor->setFixedHeight(300);
}

ShaderWidget::~ShaderWidget()
{

}

void ShaderWidget::on_vertex_margin_clicked(int nmargin, int nline, Qt::KeyboardModifiers modifiers) {
    // Toggle marker for the line the margin was clicked on
    if (vsciEditor->markersAtLine(nline) != 0) {
        vsciEditor->markerDelete(nline, BREAK_MARKER_NUM);
    } else {
        vsciEditor->markerAdd(nline, BREAK_MARKER_NUM);
    }
}

void ShaderWidget::on_fragment_margin_clicked(int nmargin, int nline, Qt::KeyboardModifiers modifiers) {
    // Toggle marker for the line the margin was clicked on
    if (fsciEditor->markersAtLine(nline) != 0) {
        fsciEditor->markerDelete(nline, BREAK_MARKER_NUM);
    } else {
        fsciEditor->markerAdd(nline, BREAK_MARKER_NUM);
    }
}

