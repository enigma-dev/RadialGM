#ifndef UTILITY_H
#define UTILITY_H

#endif  // UTILITY_H

#include <QFileDialog>
#include <QPainter>

class ImageDialog : public QFileDialog {
 public:
  ImageDialog(QWidget* parent, QString xmlExtension, bool writer = false);
};

void paint_grid(QPainter& painter, int width, int height, int gridHorSpacing, int gridVertSpacing, int gridHorOff,
                int gridVertOff, int gridWidth, int gridHeight);
