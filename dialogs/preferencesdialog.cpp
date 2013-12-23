/**
* @file  preferencesdialog.cpp
* @brief Source implementing a class for a preferences dialog.
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

#include <QStyleFactory>
#include <QPushButton>

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    QPushButton* applyButton = ui->buttonBox->button(QDialogButtonBox::Apply);
    connect(applyButton, SIGNAL(clicked()), this, SLOT(applyChanges()));

    QStringList styles = QStyleFactory::keys();
    for (int i = 0; i < styles.size(); ++i) {
        ui->comboBox->addItem(styles.at(i));
    }
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::applyChanges() {
    QApplication::setStyle(QStyleFactory::create(ui->comboBox->currentText().toLower()));
}
