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

    textEditor = new QTextEdit();
    this->setCentralWidget(textEditor);

    QAction* saveAction = new QAction(QIcon(":/icons/actions/save.png"), "Save to file", NULL);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(fileSave()));
    QAction* openAction = new QAction(QIcon(":/icons/actions/open.png"), "Open from file", NULL);
    connect(openAction, SIGNAL(triggered()), this, SLOT(fileOpen()));
    QAction* printAction = new QAction(QIcon(":/icons/actions/print.png"), "Print", NULL);
    connect(printAction, SIGNAL(triggered()), this, SLOT(filePrint()));
    QAction* printPreviewAction = new QAction(QIcon(":/icons/actions/print.png"), "Print Preview", NULL);
    connect(printPreviewAction, SIGNAL(triggered()), this, SLOT(filePrintPreview()));

    QAction* undoAction = new QAction(QIcon(":/icons/actions/undo.png"), "Undo", NULL);
    connect(undoAction, SIGNAL(triggered()), textEditor, SLOT(undo()));
    QAction* redoAction = new QAction(QIcon(":/icons/actions/redo.png"), "Redo", NULL);
    connect(redoAction, SIGNAL(triggered()), textEditor, SLOT(redo()));

    QAction* cutAction = new QAction(QIcon(":/icons/actions/cut.png"), "Cut", NULL);
    connect(cutAction, SIGNAL(triggered()), textEditor, SLOT(cut()));
    QAction* copyAction = new QAction(QIcon(":/icons/actions/copy.png"), "Copy", NULL);
    connect(copyAction, SIGNAL(triggered()), textEditor, SLOT(copy()));
    QAction* pasteAction = new QAction(QIcon(":/icons/actions/paste.png"), "Paste", NULL);
    connect(pasteAction, SIGNAL(triggered()), textEditor, SLOT(paste()));

    QAction* boldAction = new QAction(QIcon(":/icons/actions/text-bold.png"), "Bold", NULL);
    connect(boldAction, SIGNAL(triggered()), textEditor, SLOT(paste()));
    QAction* italicAction = new QAction(QIcon(":/icons/actions/text-italic.png"), "Italic", NULL);
    connect(italicAction, SIGNAL(triggered()), textEditor, SLOT(paste()));
    QAction* underlineAction = new QAction(QIcon(":/icons/actions/text-underlined.png"), "Underline", NULL);
    connect(underlineAction, SIGNAL(triggered()), textEditor, SLOT(paste()));

    QAction* leftAction = new QAction(QIcon(":/icons/actions/text-left.png"), "Left Align", NULL);
    connect(leftAction, SIGNAL(triggered()), textEditor, SLOT(paste()));
    QAction* centerAction = new QAction(QIcon(":/icons/actions/text-center.png"), "Center Align", NULL);
    connect(centerAction, SIGNAL(triggered()), textEditor, SLOT(paste()));
    QAction* rightAction = new QAction(QIcon(":/icons/actions/text-right.png"), "Right Align", NULL);
    connect(rightAction, SIGNAL(triggered()), textEditor, SLOT(paste()));

    QMenuBar* menuBar = new QMenuBar();
    QMenu* fileMenu = new QMenu("&File");
    fileMenu->addAction(QIcon(":/icons/actions/accept.png"), "Save Changes");
    fileMenu->addSeparator();
    fileMenu->addAction(saveAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(printAction);
    fileMenu->addAction(printPreviewAction);
    menuBar->addMenu(fileMenu);
    QMenu* editMenu = new QMenu("&Edit");
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    menuBar->addMenu(editMenu);
    QMenu* formatMenu = new QMenu("&Format");
    menuBar->addMenu(formatMenu);
    this->setMenuBar(menuBar);

    QToolBar* editToolbar = new QToolBar();
    editToolbar->addAction(QIcon(":/icons/actions/accept.png"), "Save Changes");
    editToolbar->addSeparator();
    editToolbar->addAction(saveAction);
    editToolbar->addAction(openAction);
    editToolbar->addAction(printAction);
    editToolbar->addSeparator();

    editToolbar->addAction(undoAction);
    editToolbar->addAction(redoAction);
    editToolbar->addSeparator();

    editToolbar->addAction(cutAction);
    editToolbar->addAction(copyAction);
    editToolbar->addAction(pasteAction);

    editToolbar->addSeparator();

    editToolbar->addAction(boldAction);
    editToolbar->addAction(italicAction);
    editToolbar->addAction(underlineAction);

    editToolbar->addSeparator();

    editToolbar->addAction(leftAction);
    editToolbar->addAction(centerAction);
    editToolbar->addAction(rightAction);

    this->addToolBar(editToolbar);
}

GameInfoEditor::~GameInfoEditor()
{

}

void GameInfoEditor::fileOpen() {
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

void GameInfoEditor::fileSave() {
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

void GameInfoEditor::filePrint()
{
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (textEditor->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted)
        textEditor->print(&printer);
    delete dlg;
#endif
}

void GameInfoEditor::filePrintPreview()
{
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
#endif
}

void GameInfoEditor::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    textEditor->print(printer);
#endif
}

void GameInfoEditor::textBold()
{
    QTextCharFormat fmt;
   // fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void GameInfoEditor::textUnderline()
{
    QTextCharFormat fmt;
    //fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void GameInfoEditor::textItalic()
{
    QTextCharFormat fmt;
   // fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void GameInfoEditor::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEditor->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEditor->mergeCurrentCharFormat(format);
}
