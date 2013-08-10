/**
* @file  modelwidget.cpp
* @brief Source implementing a class to create a model editor.
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

#include "modelwidget.h"

ModelWidget::ModelWidget(QWidget *parent) :
    QWidget(parent)
{

    this->setWindowTitle("Model");
    this->setWindowIcon(QIcon(":/resources/icons/resources/background.png"));

    QVBoxLayout* propertiesLayout = new QVBoxLayout();

    QPushButton* loadButton = new QPushButton("Load", this);
    loadButton->setIcon(QIcon(":/icons/actions/open.png"));
    QPushButton* editButton = new QPushButton("Edit", this);
    QPushButton* saveButton = new QPushButton("Save", this);
    saveButton->setIcon(QIcon(":/icons/actions/accept.png"));

    QLabel* nameLabel = new QLabel("Name: ");
    QLineEdit* nameEdit = new QLineEdit("mod_0", this);
    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);

    QLabel* sizeLabel = new QLabel("Vertices: 0 Primitives: 0");

    QCheckBox* smoothCheckBox = new QCheckBox("Invert Normals", this);
    QCheckBox* transparentCheckBox = new QCheckBox("Flip texture coordinates", this);
    QCheckBox* preloadCheckBox = new QCheckBox("Preload", this);
    QCheckBox* tilesetCheckBox = new QCheckBox("Tileset", this);

    QGraphicsView* backgroundPreviewer = new QGraphicsView(this);


    propertiesLayout->addLayout(nameLayout);
    propertiesLayout->addWidget(loadButton);
    propertiesLayout->addWidget(editButton);
    propertiesLayout->addWidget(sizeLabel);
    propertiesLayout->addWidget(smoothCheckBox);
    propertiesLayout->addWidget(transparentCheckBox);
    propertiesLayout->addWidget(preloadCheckBox);
    propertiesLayout->addWidget(tilesetCheckBox);
    propertiesLayout->addWidget(saveButton);
    QSplitter* horizontalSplitter = new QSplitter(this);
    QWidget* propertiesWidget = new QWidget(this, Qt::WindowTitleHint);
    QFormLayout* propertiesFormLayout = new QFormLayout();
    propertiesFormLayout->setLayout(0, QFormLayout::SpanningRole, propertiesLayout);
    propertiesWidget->setLayout(propertiesFormLayout);

    horizontalSplitter->addWidget(propertiesWidget);
    horizontalSplitter->addWidget(backgroundPreviewer);

    QVBoxLayout* verticalLayout = new QVBoxLayout();
    verticalLayout->addWidget(horizontalSplitter);
    verticalLayout->setContentsMargins(2, 2, 2, 2);
    this->setLayout(verticalLayout);
}

ModelWidget::~ModelWidget()
{

}
