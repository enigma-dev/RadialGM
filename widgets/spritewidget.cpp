/**
* @file  spritewidget.cpp
* @brief Header implementing a class to create a sprite editor.
*
* @section License
*
* Copyright (C) 2013 Robert B. Colton
* This file is a part of the RadialGM IDE.
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
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>
#include <QListWidget>

SpriteWidget::SpriteWidget(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowTitle("Sprite");
    this->setWindowIcon(QIcon(":/resources/icons/resources/sprite.png"));

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(3, 3, 3, 3);

    QToolBar* toolBar = new QToolBar();
    toolBar->addAction(QIcon(":/icons/actions/accept.png"), "Save");
    toolBar->addSeparator();
    toolBar->addAction(QIcon(":/icons/actions/save.png"), "Save to file");
    toolBar->addAction(QIcon(":/icons/actions/open.png"), "Load from file");
    toolBar->setStyleSheet(" QToolBar { height: 18px; width: 18px; icon-size: 18px; } ");
    mainLayout->addWidget(toolBar, 0, 0);

    QSplitter* hSplitter = new QSplitter();
    hSplitter->addWidget(makePropertyPanel());
    hSplitter->addWidget(makeEditorPanel());
    mainLayout->addWidget(hSplitter, 0, 0);

    QStatusBar* statusBar = new QStatusBar();
    statusBar->showMessage("Width: 0 | Height: 0 | Images: 0 | Memory: 0 B | Zoom: 100%");
    statusBar->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    mainLayout->addWidget(statusBar);
    this->setLayout(mainLayout);
}

SpriteWidget::~SpriteWidget()
{

}

QWidget* SpriteWidget::makePropertyPanel() {
    QWidget* panel = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();

    QVBoxLayout* cbLayout = new QVBoxLayout();

    QCheckBox* smoothCB = new QCheckBox("Smooth Edges");
    cbLayout->addWidget(smoothCB);
    QCheckBox* preloadCB = new QCheckBox("Preload Texture");
    cbLayout->addWidget(preloadCB);
    QCheckBox* transparentCB = new QCheckBox("Transparency Pixel");
    cbLayout->addWidget(transparentCB);

    layout->addLayout(cbLayout);

    QGroupBox* centerBox = new QGroupBox("Origin");
    centerBox->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    QVBoxLayout* centerVLayout = new QVBoxLayout();
    QHBoxLayout* centerHLayout = new QHBoxLayout();
    centerHLayout->addWidget(new QLabel("X:"));
    QSpinBox* xBox = new QSpinBox();
    centerHLayout->addWidget(xBox);
    centerHLayout->addWidget(new QLabel("Y:"));
    QSpinBox* yBox = new QSpinBox();
    centerHLayout->addWidget(yBox);
    centerVLayout->addLayout(centerHLayout);
    QPushButton* centerButton = new QPushButton("Center");
    centerVLayout->addWidget(centerButton);
    centerBox->setLayout(centerVLayout);
    layout->addWidget(centerBox);

    panel->setLayout(layout);
    panel->setContentsMargins(0,0,0,0);
    return panel;
}

QWidget* SpriteWidget::makeEditorPanel() {
    QSplitter* splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);

    QGraphicsView* graphicsView = new QGraphicsView();

    splitter->addWidget(graphicsView);

    QWidget* subimagePanel = new QWidget();
    subimagePanel->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout* verticalLayout = new QVBoxLayout();

    QToolBar* subimageToolBar = new QToolBar();
    subimageToolBar->addAction(QIcon(":/icons/actions/undo.png"), "Undo");
    subimageToolBar->addAction(QIcon(":/icons/actions/redo.png"), "Redo");
    subimageToolBar->addSeparator();
    subimageToolBar->addAction(QIcon(":/icons/actions/cut.png"), "Cut");
    subimageToolBar->addAction(QIcon(":/icons/actions/copy.png"), "Copy");
    subimageToolBar->addAction(QIcon(":/icons/actions/paste.png"), "Paste");
    subimageToolBar->setStyleSheet(" QToolBar { height: 18px; width: 18px; icon-size: 18px; } ");
    QListWidget* subimageList = new QListWidget();
    subimageList->addItem("wtf");

    verticalLayout->addWidget(subimageToolBar);
    verticalLayout->addWidget(subimageList);

    subimagePanel->setLayout(verticalLayout);
    splitter->addWidget(subimagePanel);

    return splitter;
}
