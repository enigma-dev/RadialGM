/**
* @file  spritewidget.cpp
* @brief Header implementing a class to create a sprite editor.
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

#include "spritewidget.h"
#include "ui_spritewidget.h"

SpriteWidget::SpriteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpriteWidget)
{
    ui->setupUi(this);

    this->setWindowTitle("Sprite");
    this->setWindowIcon(QIcon(":/resources/icons/resources/sprite.png"));
}

SpriteWidget::~SpriteWidget()
{
    delete ui;
}
