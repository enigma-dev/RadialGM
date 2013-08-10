/**
* @file  pathwidget.cpp
* @brief Source implementing a class to create a path editor.
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

#include "pathwidget.h"
#include "ui_pathwidget.h"

PathWidget::PathWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PathWidget)
{
    QVBoxLayout* layout = new QVBoxLayout();
    this->setLayout(layout);
    editToolbar = new QToolBar(this);
    editToolbar->setFloatable(true);
    editToolbar->setIconSize(QSize(16, 16));
    editToolbar->addAction(QIcon(":/icons/actions/accept.png"), "Save");
    editToolbar->addSeparator();
    editToolbar->addWidget(new QLabel("Snap X:"));
    QLineEdit* snapXEdit = new QLineEdit("16", this);
    snapXEdit->sizePolicy().setHorizontalPolicy(QSizePolicy::Minimum);
    editToolbar->addWidget(snapXEdit);
    editToolbar->addWidget(new QLabel("Snap Y:"));
    editToolbar->addWidget(new QLineEdit("16", this));
    editToolbar->addWidget(new QCheckBox("Grid", this));

    layout->addWidget(editToolbar);
    ui->setupUi(this);
    layout->addWidget(ui->splitter);
    layout->setContentsMargins(2, 2, 2, 2);
}

PathWidget::~PathWidget()
{
    delete ui;
}
