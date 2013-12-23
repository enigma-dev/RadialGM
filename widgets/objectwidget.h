/**
* @file  objectwidget.h
* @brief Header implementing a class to create an object editor.
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

#ifndef OBJECTWIDGET_H
#define OBJECTWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>

class ObjectWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ObjectWidget(QWidget *parent = 0);
    ~ObjectWidget();
    
    QSplitter* mainSplitter;
    QLineEdit* nameEdit;
    QHBoxLayout* mainLayout;
    QTreeWidget* eventTree;

    void addEvent(QString name, QIcon icon);

private:

};

#endif // OBJECTWIDGET_H
