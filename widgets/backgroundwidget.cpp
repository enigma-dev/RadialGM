/**
* @file  backgroundwidget.cpp
* @brief Source implementing a class to create a background editor.
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

#include "backgroundwidget.h"

BackgroundWidget::BackgroundWidget(QWidget *parent) :
    QWidget(parent)
{

    this->setWindowTitle("Background");
    this->setWindowIcon(QIcon(":/resources/icons/resources/background.png"));

    zoomInAction = new QAction(QIcon(":/icons/actions/zoom-in.png"), "Zoom In", NULL);
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
    zoomOutAction = new QAction(QIcon(":/icons/actions/zoom-out.png"), "Zoom Out", NULL);
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
    QAction* openAction = new QAction(QIcon(":/icons/actions/open.png"), "Load from file", NULL);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
    QAction* saveAction = new QAction(QIcon(":/icons/actions/save.png"), "Save to file", NULL);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
    QAction* printAction = new QAction(QIcon(":/icons/actions/print.png"), "Print", NULL);
    connect(printAction, SIGNAL(triggered()), this, SLOT(print()));

    QVBoxLayout* propertiesLayout = new QVBoxLayout();

    QLabel* nameLabel = new QLabel("Name: ");
    QLineEdit* nameEdit = new QLineEdit("bg_0", this);

    QToolBar* toolBar = new QToolBar();
    toolBar->addAction(QIcon(":/icons/actions/accept.png"), "Save Changes");
    toolBar->addAction(openAction);
    toolBar->addAction(saveAction);
    toolBar->addAction(printAction);

    toolBar->addSeparator();
    toolBar->addAction(zoomInAction);
    toolBar->addAction(zoomOutAction);
    toolBar->addSeparator();
    toolBar->addWidget(nameLabel);
    toolBar->addWidget(nameEdit);

    toolBar->setStyleSheet(" QToolBar { height: 18px; width: 18px; icon-size: 18px; } ");

    QCheckBox* smoothCheckBox = new QCheckBox("Smooth", this);
    QCheckBox* transparentCheckBox = new QCheckBox("Transparent", this);
    QCheckBox* preloadCheckBox = new QCheckBox("Preload", this);
    QCheckBox* tilesetCheckBox = new QCheckBox("Tileset", this);

    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);


    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    propertiesLayout->addWidget(smoothCheckBox);
    propertiesLayout->addWidget(transparentCheckBox);
    propertiesLayout->addWidget(preloadCheckBox);
    propertiesLayout->addWidget(tilesetCheckBox);
    QSplitter* horizontalSplitter = new QSplitter(this);
    QWidget* propertiesWidget = new QWidget(this, Qt::WindowTitleHint);
    QFormLayout* propertiesFormLayout = new QFormLayout();
    propertiesFormLayout->setLayout(0, QFormLayout::SpanningRole, propertiesLayout);
    propertiesWidget->setLayout(propertiesFormLayout);

    horizontalSplitter->addWidget(propertiesWidget);
    horizontalSplitter->addWidget(scrollArea);
    horizontalSplitter->setStretchFactor(0, 0);
    horizontalSplitter->setStretchFactor(1, 1);

    QVBoxLayout* verticalLayout = new QVBoxLayout();
    verticalLayout->addWidget(toolBar);
    verticalLayout->addWidget(horizontalSplitter);
    QStatusBar* statusBar = new QStatusBar();
    statusBar->showMessage("Width: 0 | Height: 0 | Memory: 0 B | Zoom: 100%");
    statusBar->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    verticalLayout->addWidget(statusBar);
    verticalLayout->setContentsMargins(2, 2, 2, 2);
    this->setLayout(verticalLayout);
}

BackgroundWidget::~BackgroundWidget()
{

}

void BackgroundWidget::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                    tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Image Viewer"),
                                     tr("Cannot load %1.").arg(fileName));
            return;
        }
        imageLabel->setPixmap(QPixmap::fromImage(image));

        scaleFactor = 1.0;

        //printAction->setEnabled(true);
        //fitToWindowAct->setEnabled(true);
       // updateActions();

        //if (!fitToWindowAct->isChecked())
           // imageLabel->adjustSize();
    }
}

void BackgroundWidget::saveFile() {

}

void BackgroundWidget::print()
{
    Q_ASSERT(imageLabel->pixmap());
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrintDialog dialog(&printer, this);
    if (dialog.exec()) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = imageLabel->pixmap()->size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(imageLabel->pixmap()->rect());
        painter.drawPixmap(0, 0, *imageLabel->pixmap());
    }
#endif
}

void BackgroundWidget::zoomIn()
{
    scaleImage(1.25);
}

void BackgroundWidget::zoomOut()
{
    scaleImage(0.8);
}

void BackgroundWidget::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAction->setEnabled(scaleFactor < 3.0);
    zoomOutAction->setEnabled(scaleFactor > 0.333);
}

void BackgroundWidget::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
