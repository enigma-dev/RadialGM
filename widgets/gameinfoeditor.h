/**
* @file  gameinfoeditor.h
* @brief Header implementing a class to create a game information rich text editor.
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

#ifndef GAMEINFOEDITOR_H
#define GAMEINFOEDITOR_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QTextEdit>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#endif

class GameInfoEditor : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit GameInfoEditor(QWidget *parent = 0);
    ~GameInfoEditor();
    
public slots:
    void fileOpen();
    void fileSave();
    void filePrint();
    void filePrintPreview();

    void textBold();
    void textItalic();
    void textUnderline();

private:
    QTextEdit* textEditor;
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

private slots:
    void printPreview(QPrinter *printer);
};

#endif // GAMEINFOEDITOR_H
