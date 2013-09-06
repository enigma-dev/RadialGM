/**
* @file  timelinewidget.cpp
* @brief Header implementing a class to create a timeline editor.
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

#include "timelinewidget.h"

TimelineWidget::TimelineWidget(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowTitle("Timeline");
    this->setWindowIcon(QIcon(":/resources/icons/resources/timeline.png"));

    QFormLayout* propertiesLayout = new QFormLayout(this);
    propertiesLayout->setContentsMargins(2, 2, 2, 2);

    QHBoxLayout* nameLayout = new QHBoxLayout();
    QLabel* nameLabel = new QLabel("Name: ", this);
    QLineEdit* nameEdit = new QLineEdit("tml_0", this);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    propertiesLayout->addItem(nameLayout);

    editButton = new QPushButton("Edit", this);
    propertiesLayout->addWidget(editButton);

    QGridLayout* momButtonsLayout = new QGridLayout();
    addButton = new QPushButton("Add", this);
    momButtonsLayout->addWidget(addButton, 0, 0);
    changeButton = new QPushButton("Change", this);
    momButtonsLayout->addWidget(changeButton, 0, 1);
    deleteButton = new QPushButton("Delete", this);
    momButtonsLayout->addWidget(deleteButton, 1, 0);
    duplicateButton = new QPushButton("Duplicate", this);
    momButtonsLayout->addWidget(duplicateButton, 1, 1);
    shiftButton = new QPushButton("Shift", this);
    momButtonsLayout->addWidget(shiftButton, 2, 0);
    mergeButton = new QPushButton("Merge", this);
    momButtonsLayout->addWidget(mergeButton, 2, 1);
    propertiesLayout->addItem(momButtonsLayout);

    clearButton = new QPushButton("Clear", this);
    propertiesLayout->addWidget(clearButton);
    infoButton = new QPushButton("Information", this);
    infoButton->setIcon(QIcon(":/resources/icons/resources/info.png"));
    propertiesLayout->addWidget(infoButton);
    saveButton = new QPushButton("Save", this);
    saveButton->setIcon(QIcon(":/icons/actions/accept.png"));
    propertiesLayout->addWidget(saveButton);

    QTreeWidget* momentsTree = new QTreeWidget(this);
    momentsTree->setHeaderLabel("Moments");

    QSplitter* horizontalSplitter = new QSplitter();
    QWidget* propertiesWidget = new QWidget(this, Qt::WindowTitleHint);
    propertiesWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    propertiesWidget->setLayout(propertiesLayout);
    horizontalSplitter->addWidget(propertiesWidget);
    horizontalSplitter->addWidget(momentsTree);

    QVBoxLayout* timelineLayout = new QVBoxLayout();
    timelineLayout->setContentsMargins(2, 2, 2, 2);
    timelineLayout->addWidget(horizontalSplitter);

    this->setLayout(timelineLayout);
}

TimelineWidget::~TimelineWidget()
{

}
