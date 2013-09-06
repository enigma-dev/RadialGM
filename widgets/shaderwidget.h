/**
* @file  shaderwidget.h
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

#ifndef SCRIPTFORM_H
#define SCRIPTFORM_H

#include <QWidget>
#include <QGridLayout>
#include <QIcon>
#include <QToolBar>
#include <QLineEdit>
#include <QLabel>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercpp.h>

class ShaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ShaderWidget(QWidget *parent = 0);
    ~ShaderWidget();

private:

    QToolBar* editToolbar;
    QsciScintilla* vsciEditor; // Scintilla editor for the vertex code
    QsciScintilla* fsciEditor; // Scintilla editor for the fragment code

    int BREAK_MARKER_NUM;

    QMenu* editMenu;

public slots:
    void on_vertex_margin_clicked(int nmargin, int nline, Qt::KeyboardModifiers modifiers);
    void on_fragment_margin_clicked(int nmargin, int nline, Qt::KeyboardModifiers modifiers);

};

#endif // SCRIPTFORM_H
