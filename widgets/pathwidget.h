/**
* @file  pathwidget.h
* @brief Header implementing a class to create a path editor.
*
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

#ifndef PATHWIDGET_H
#define PATHWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QGraphicsView>
#include <QListWidget>

namespace Ui {
class PathWidget;
}

class PathWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit PathWidget(QWidget *parent = 0);
    ~PathWidget();
    
    QToolBar* editToolbar;

private:
    Ui::PathWidget *ui;
};

#endif // PATHWIDGET_H
