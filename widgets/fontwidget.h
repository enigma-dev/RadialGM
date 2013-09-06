/**
* @file  fontwidget.h
* @brief Header implementing a class to create a font editor.
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

#ifndef FONTWIDGET_H
#define FONTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

class FontWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit FontWidget(QWidget *parent = 0);
    ~FontWidget();
    
private:

    QLabel* nameLabel;
    QLineEdit* nameEdit;
    QLabel* fontLabel;
    QFontComboBox* fontCombo;
    QLabel* sizeLabel;
    QLineEdit* sizeEdit;
    QCheckBox* boldCheckBox;
    QCheckBox* italicCheckBox;
    QLabel* antialiasLabel;
    QComboBox* antialiasCombo;
    QGroupBox* charRangeGroup;
    QLineEdit* minCharEdit;
    QLineEdit* maxCharEdit;
    QLabel* mintomaxLabel;
    QPushButton* normalCharButton;
    QPushButton* allCharButton;
    QPushButton* digitsCharButton;
    QPushButton* lettersCharButton;
    QTextEdit* previewEdit;
    QPushButton* saveButton;

};

#endif // FONTWIDGET_H
