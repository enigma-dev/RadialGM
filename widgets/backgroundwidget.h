/**
* @file  backgroundwidget.h
* @brief Header implementing a class to create a background editor.
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


#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QGraphicsView>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
#include <QScrollArea>
#include <QScrollBar>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>

class BackgroundWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit BackgroundWidget(QWidget *parent = 0);
    ~BackgroundWidget();

private:
#ifndef QT_NO_PRINTER
    QPrinter printer;
#endif

    QAction* zoomInAction;
    QAction* zoomOutAction;

    QLabel* nameLabel;
    QLineEdit* nameEdit;
    QLabel* sizeLabel;

    QCheckBox* smoothCheckBox;
    QCheckBox* transparentCheckBox;
    QCheckBox* preloadCheckBox;
    QCheckBox* tilesetCheckBox;

    QSplitter* horizontalSplitter;
    QLabel* imageLabel;
    QScrollArea *scrollArea;
    double scaleFactor;

    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

private slots:
    void zoomIn();
    void zoomOut();
    void openFile();
    void saveFile();
    void print();

};

#endif // BACKGROUNDWIDGET_H
