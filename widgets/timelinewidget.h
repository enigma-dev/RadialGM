/**
* @file  timelinewidget.h
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

#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QGridLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>

class TimelineWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit TimelineWidget(QWidget *parent = 0);
    ~TimelineWidget();

private:

    QLineEdit* nameEdit;
    QPushButton* editButton;
    QPushButton* addButton;
    QPushButton* changeButton;
    QPushButton* deleteButton;
    QPushButton* duplicateButton;
    QPushButton* shiftButton;
    QPushButton* mergeButton;
    QPushButton* clearButton;
    QPushButton* infoButton;
    QPushButton* saveButton;
    QTreeWidget* momentsTree;

};

#endif // TIMELINEWIDGET_H
