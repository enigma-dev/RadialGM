/**
* @file  fontwidget.cpp
* @brief Source implementing a class to create a font editor.
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

#include "fontwidget.h"

FontWidget::FontWidget(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowIcon(QIcon(":/resources/icons/resources/font.png"));
    this->setWindowTitle("Font");

    QVBoxLayout* widgetLayout = new QVBoxLayout();
    widgetLayout->setContentsMargins(3, 3, 3, 3);

    QFormLayout* editLayout = new QFormLayout();
    nameLabel = new QLabel("Name: ");
    nameEdit = new QLineEdit("fnt_0");
    editLayout->addRow(nameLabel, nameEdit);
    fontLabel = new QLabel("Font: ");
    fontCombo = new QFontComboBox();
    editLayout->addRow(fontLabel, fontCombo);
    sizeLabel = new QLabel("Size: ");
    sizeEdit = new QLineEdit("12");
    editLayout->addRow(sizeLabel, sizeEdit);
    widgetLayout->addLayout(editLayout);

    QHBoxLayout* styleLayout = new QHBoxLayout();
    boldCheckBox = new QCheckBox("Bold");
    italicCheckBox = new QCheckBox("Italic");
    antialiasLabel = new QLabel("Antialias: ");
    antialiasCombo = new QComboBox();
    antialiasCombo->addItem("Off");
    antialiasCombo->addItem("1");
    antialiasCombo->addItem("2");
    antialiasCombo->addItem("3");
    styleLayout->addWidget(boldCheckBox);
    styleLayout->addWidget(italicCheckBox);
    styleLayout->addWidget(antialiasLabel);
    styleLayout->addWidget(antialiasCombo);
    widgetLayout->addLayout(styleLayout);

    charRangeGroup = new QGroupBox("Character Range");
    QVBoxLayout* charRangeLayout = new QVBoxLayout();

    QHBoxLayout* charRangeHLayout = new QHBoxLayout();
    minCharEdit = new QLineEdit("32");
    maxCharEdit = new QLineEdit("127");
    mintomaxLabel = new QLabel("to");
    charRangeHLayout->addWidget(minCharEdit);
    charRangeHLayout->addWidget(mintomaxLabel);
    charRangeHLayout->addWidget(maxCharEdit);
    charRangeLayout->addLayout(charRangeHLayout);

    QGridLayout* charRangeGLayout = new QGridLayout();
    normalCharButton = new QPushButton("Normal");
    charRangeGLayout->addWidget(normalCharButton, 0, 0);
    allCharButton = new QPushButton("All");
    charRangeGLayout->addWidget(allCharButton, 0, 1);
    digitsCharButton = new QPushButton("Digits");
    charRangeGLayout->addWidget(digitsCharButton, 1, 0);
    lettersCharButton = new QPushButton("Letters");
    charRangeGLayout->addWidget(lettersCharButton, 1, 1);
    charRangeLayout->addLayout(charRangeGLayout);
    charRangeGroup->setLayout(charRangeLayout);
    charRangeGroup->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    widgetLayout->addWidget(charRangeGroup);

    previewEdit = new QTextEdit("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz");
    widgetLayout->addWidget(previewEdit);
    saveButton = new QPushButton("Save");
    saveButton->setIcon(QIcon(":/icons/actions/accept.png"));
    widgetLayout->addWidget(saveButton);

    this->setLayout(widgetLayout);
    this->resize(50, 50);
}

FontWidget::~FontWidget()
{

}
