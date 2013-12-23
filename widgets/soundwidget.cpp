/**
* @file  soundwidget.cpp
* @brief Header implementing a class to create a sound editor.
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

#include "soundwidget.h"

SoundWidget::SoundWidget(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowTitle("Sound");
    this->setWindowIcon(QIcon(":/resources/icons/resources/sound.png"));

    editToolbar = new QToolBar(this);
    editToolbar->setFloatable(true);
    editToolbar->setIconSize(QSize(16, 16));
    editToolbar->addAction(QIcon(":/icons/actions/accept.png"), "Save");
    editToolbar->addSeparator();
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(editToolbar);
    //layout->addLayout(ui->verticalLayout_2);
    this->setLayout(vLayout);
    vLayout->setContentsMargins(2, 2, 2, 2);

    QHBoxLayout* hLayout = new QHBoxLayout();
    QLabel* filenameLabel = new QLabel("Filename:");
    hLayout->addWidget(filenameLabel);

    vLayout->addLayout(hLayout, 1);

    hLayout = new QHBoxLayout();
    QGroupBox* kindGroup = new QGroupBox("Kind", this);
    QVBoxLayout* kindLayout = new QVBoxLayout();
    QRadioButton* normalRadio = new QRadioButton("Normal", this);
    normalRadio->setChecked(true);
    kindLayout->addWidget(normalRadio);
    QRadioButton* backgroundRadio = new QRadioButton("Background", this);
    kindLayout->addWidget(backgroundRadio);
    QRadioButton* threedRadio = new QRadioButton("3D Sound", this);
    kindLayout->addWidget(threedRadio);
    QRadioButton* multimediaRadio = new QRadioButton("Multimedia", this);
    kindLayout->addWidget(multimediaRadio);
    kindGroup->setLayout(kindLayout);
    hLayout->addWidget(kindGroup);
    QGroupBox* effectGroup = new QGroupBox("Effect", this);
    QGridLayout* effectLayout = new QGridLayout();
    QCheckBox* chorusCheckbox = new QCheckBox("Chorus", this);
    effectLayout->addWidget(chorusCheckbox);
    QCheckBox* flangerCheckbox = new QCheckBox("Flanger", this);
    effectLayout->addWidget(flangerCheckbox);
    QCheckBox* reverbCheckbox = new QCheckBox("Reverb", this);
    effectLayout->addWidget(reverbCheckbox);
    QCheckBox* echoCheckbox = new QCheckBox("Echo", this);
    effectLayout->addWidget(echoCheckbox, 0, 1);
    QCheckBox* gargleCheckbox = new QCheckBox("Gargle", this);
    effectLayout->addWidget(gargleCheckbox, 1, 1);
    effectGroup->setLayout(effectLayout);
    hLayout->addWidget(effectGroup);
    vLayout->addLayout(hLayout, 1);

    QLabel* volumeLabel = new QLabel("Volume: 75");
    QSlider* volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setTickInterval(10);
    volumeSlider->setTickPosition(QSlider::TicksBelow);
    volumeSlider->setMaximum(100);
    volumeSlider->setMinimum(0);
    volumeSlider->setValue(75);
    vLayout->addWidget(volumeLabel);
    vLayout->addWidget(volumeSlider);
    QLabel* panLabel = new QLabel("Pan: 0");
    QSlider* panSlider = new QSlider(Qt::Horizontal, this);
    panSlider->setTickInterval(20);
    panSlider->setTickPosition(QSlider::TicksBelow);
    panSlider->setMaximum(100);
    panSlider->setMinimum(-100);
    vLayout->addWidget(panLabel);
    vLayout->addWidget(panSlider);
}

SoundWidget::~SoundWidget()
{

}
