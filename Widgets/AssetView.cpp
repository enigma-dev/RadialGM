#include "AssetView.h"

AssetView::AssetView(QWidget* parent) : QWidget(parent), zoom(1) {}

void AssetView::SetZoom(qreal zoom) {
  if (zoom > 3200) zoom = 3200;
  if (zoom < 0.0625) zoom = 0.0625;
  this->zoom = zoom;
  setFixedSize(sizeHint() * zoom);
}

const qreal& AssetView::GetZoom() const { return zoom; }

void AssetView::paintGrid(QPainter& painter, int width, int height, int gridHorSpacing, int gridVertSpacing,
                          int gridHorOff, int gridVertOff, int gridWidth, int gridHeight) {
  // do not draw zero-area grids
  if (width == 0 || height == 0) return;
  if (gridWidth == 0 || gridHeight == 0) return;

  // save the painter state so we can restore it before returning
  painter.save();

  // xor the destination color
  painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
  // this pen not only sets the color but also gives us perfectly square rectangles
  QPen pen(Qt::white, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
  painter.setPen(pen);

  if (gridHorSpacing != 0 || gridVertSpacing != 0) {
    painter.translate(0.5, 0.5);
    const int horStep = gridWidth + gridHorSpacing;
    const int verStep = gridHeight + gridVertSpacing;

    for (int x = gridHorOff; x < width; x += horStep) {
      for (int y = gridVertOff; y < height; y += verStep) {
        painter.drawRect(x, y, gridWidth - 1, gridHeight - 1);
      }
    }
  } else {
    for (int x = gridHorOff; x <= width; x += gridWidth) painter.drawLine(x, 0, x, height);
    for (int y = gridVertOff; y <= height; y += gridHeight) painter.drawLine(0, y, width, y);
  }

  painter.restore();
}
