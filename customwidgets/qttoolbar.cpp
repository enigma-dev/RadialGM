/**
* @file  qttoolbar.cpp
* @brief Source implementing a class for an advanced toolbar.
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

#include "qttoolbar.h"

QtToolBar::QtToolBar(QWidget *parent) :
    QToolBar(parent)
{
    defActionSize = QSize(26, 26);
}

QtToolBar::~QtToolBar()
{

}

void QtToolBar::addActionSized(QAction* action) {
    this->addAction(action);
    QWidget* actWidget;
    actWidget = this->widgetForAction(action);
    actWidget->setFixedSize(defActionSize);
}

void QtToolBar::setDefaultActionSize(QSize size) {
    defActionSize = size;
}


void QtToolBar::setActionWidgetSize(QSize size) {
    QList <QAction*> acts = this->actions();
    QWidget* actWidget;
    for (int i = 0; i < acts.size(); i++)
    {
        actWidget = this->widgetForAction(acts[i]);
        actWidget->setFixedSize(size);
    }
}
