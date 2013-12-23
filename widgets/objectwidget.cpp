/**
* @file  objectwidget.cpp
* @brief Source implementing a class to create an object editor.
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

#include "objectwidget.h"

ObjectWidget::ObjectWidget(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowTitle("Object");
    this->setWindowIcon(QIcon(":/resources/icons/resources/object.png"));

    QVBoxLayout* propertiesLayout = new QVBoxLayout();
    QLabel* nameLabel = new QLabel("Name:");
    nameEdit = new QLineEdit();
    nameEdit->setText("obj_0");
    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    propertiesLayout->addLayout(nameLayout);

    QGroupBox* spriteGroup = new QGroupBox("Sprite");
    QVBoxLayout* spriteGroupLayout = new QVBoxLayout();
    spriteGroup->setLayout(spriteGroupLayout);
    QComboBox* spriteBox = new QComboBox();
    spriteBox->addItem("<no sprite>");
    spriteGroupLayout->addWidget(spriteBox);
    QHBoxLayout* spriteEditLayout = new QHBoxLayout();
    QPushButton* newSpriteButton = new QPushButton("New");
    spriteEditLayout->addWidget(newSpriteButton);
    QPushButton* editSpriteButton = new QPushButton("Edit");
    spriteEditLayout->addWidget(editSpriteButton);
    spriteGroupLayout->addLayout(spriteEditLayout);
    propertiesLayout->addWidget(spriteGroup);

    QGridLayout* settingsLayout = new QGridLayout();
    settingsLayout->addWidget(new QLabel("Mask:"), 0, 0);
    QComboBox* maskBox = new QComboBox();
    maskBox->addItem("<same as sprite>");
    settingsLayout->addWidget(maskBox, 0, 1);
    settingsLayout->addWidget(new QLabel("Parent:"), 1, 0);
    QComboBox* parentBox = new QComboBox();
    parentBox->addItem("<no parent>");
    settingsLayout->addWidget(parentBox, 1, 1);
    settingsLayout->addWidget(new QLabel("Depth:"), 2, 0);
    QLineEdit* depthEdit = new QLineEdit();
    depthEdit->setText("0");
    settingsLayout->addWidget(depthEdit, 2, 1);
    propertiesLayout->addLayout(settingsLayout);
    QGridLayout* checkBoxLayout = new QGridLayout();
    checkBoxLayout->addWidget(new QCheckBox("Visible"), 0, 0);
    checkBoxLayout->addWidget(new QCheckBox("Solid"), 0, 1);
    checkBoxLayout->addWidget(new QCheckBox("Persistent"), 1, 0);
    propertiesLayout->addLayout(checkBoxLayout);
    QPushButton* infoButton = new QPushButton(QIcon(":/icons/actions/notice.png"), "Information");
    propertiesLayout->addWidget(infoButton);
    QPushButton* saveButton = new QPushButton(QIcon(":/icons/actions/accept.png"), "Save");
    propertiesLayout->addWidget(saveButton);

    QVBoxLayout* eventsLayout = new QVBoxLayout();
    eventTree = new QTreeWidget();
    eventsLayout->addWidget(eventTree, 1, 0);
    eventsLayout->setContentsMargins(3, 3, 3, 3);
    eventsLayout->setSpacing(1);
    eventTree->setHeaderLabel("Events");
    this->addEvent("Create", QIcon(":/events/icons/events/create.png"));
    this->addEvent("Destroy", QIcon(":/events/icons/events/destroy.png"));
    this->addEvent("Normal Step", QIcon(":/events/icons/events/step.png"));
    this->addEvent("Draw", QIcon(":/events/icons/events/draw.png"));
    QHBoxLayout* eventButtonsLayout = new QHBoxLayout();
    eventButtonsLayout->setSpacing(1);
    QPushButton* modifyButton = new QPushButton("Modify");
    QPushButton* editButton = new QPushButton("Edit");
    QPushButton* deleteButton = new QPushButton("Delete");
    eventButtonsLayout->addWidget(modifyButton, 0, 0);
    eventButtonsLayout->addWidget(editButton, 0, 0);
    eventButtonsLayout->addWidget(deleteButton, 0, 0);
    eventsLayout->addLayout(eventButtonsLayout);

    mainSplitter = new QSplitter();
    QWidget* propWidget = new QWidget();
    propWidget->setLayout(propertiesLayout);
    QWidget* eventsWidget = new QWidget();
    eventsWidget->setLayout(eventsLayout);
    mainSplitter->addWidget(propWidget);
    mainSplitter->addWidget(eventsWidget);

    mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(3, 3, 3, 3);
    mainLayout->addWidget(mainSplitter, 0, 0);
    this->setLayout(mainLayout);
}

ObjectWidget::~ObjectWidget()
{

}

void ObjectWidget::addEvent(QString name, QIcon icon)
{
    QTreeWidgetItem* treeItem = new QTreeWidgetItem();
    treeItem->setText(0, name);
    treeItem->setIcon(0, icon);
    eventTree->addTopLevelItem(treeItem);
}
