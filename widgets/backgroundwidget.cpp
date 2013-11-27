/**
* @file  backgroundwidget.cpp
* @brief Source implementing a class to create a background editor.
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

#include "backgroundwidget.h"

BackgroundWidget::BackgroundWidget(QWidget *parent) :
    QWidget(parent)
{

    this->setWindowTitle("Background");
    this->setWindowIcon(QIcon(":/resources/icons/resources/background.png"));

    QVBoxLayout* propertiesLayout = new QVBoxLayout();

    QLabel* nameLabel = new QLabel("Name: ");
    QLineEdit* nameEdit = new QLineEdit("bg_0", this);

    QToolBar* toolBar = new QToolBar();
    toolBar->addAction(QIcon(":/icons/actions/accept.png"), "Save");
    toolBar->addAction(QIcon(":/icons/actions/open.png"), "Load");
    toolBar->addAction(QIcon(":/icons/actions/open.png"), "Edit");

    toolBar->addSeparator();
    toolBar->addWidget(nameLabel);
    toolBar->addWidget(nameEdit);

    toolBar->setStyleSheet(" QToolBar { height: 18px; width: 18px; icon-size: 18px; } ");

    QCheckBox* smoothCheckBox = new QCheckBox("Smooth", this);
    QCheckBox* transparentCheckBox = new QCheckBox("Transparent", this);
    QCheckBox* preloadCheckBox = new QCheckBox("Preload", this);
    QCheckBox* tilesetCheckBox = new QCheckBox("Tileset", this);

    QGraphicsView* backgroundPreviewer = new QGraphicsView(this);

    propertiesLayout->addWidget(smoothCheckBox);
    propertiesLayout->addWidget(transparentCheckBox);
    propertiesLayout->addWidget(preloadCheckBox);
    propertiesLayout->addWidget(tilesetCheckBox);
    QSplitter* horizontalSplitter = new QSplitter(this);
    QWidget* propertiesWidget = new QWidget(this, Qt::WindowTitleHint);
    QFormLayout* propertiesFormLayout = new QFormLayout();
    propertiesFormLayout->setLayout(0, QFormLayout::SpanningRole, propertiesLayout);
    propertiesWidget->setLayout(propertiesFormLayout);

    horizontalSplitter->addWidget(propertiesWidget);
    horizontalSplitter->addWidget(backgroundPreviewer);

    QVBoxLayout* verticalLayout = new QVBoxLayout();
    verticalLayout->addWidget(toolBar);
    verticalLayout->addWidget(horizontalSplitter);
    QStatusBar* statusBar = new QStatusBar();
    statusBar->showMessage("Width: 0 | Height: 0 | Memory: 0 B | Zoom: 100%");
    verticalLayout->addWidget(statusBar);
    verticalLayout->setContentsMargins(2, 2, 2, 2);
    this->setLayout(verticalLayout);
}

BackgroundWidget::~BackgroundWidget()
{

}
