/**
* @file  spritewidget.h
* @brief Header implementing a class to create a sprite editor.
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

#ifndef SPRITEWIDGET_H
#define SPRITEWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QGridLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGraphicsView>
#include <QTableWidget>
#include <QRadioButton>

namespace Ui {
class SpriteWidget;
}

class SpriteWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit SpriteWidget(QWidget *parent = 0);
    ~SpriteWidget();
    
private:
    Ui::SpriteWidget *ui;
};

#endif // SPRITEWIDGET_H
