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
  if (width == 0 || height == 0) return;
  if (gridWidth == 0 || gridHeight == 0) return;

  painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
  painter.setPen(QColor(Qt::white));

  if (gridHorSpacing != 0 || gridVertSpacing != 0) {
    for (int x = gridHorOff; x < width; x += gridWidth + gridHorSpacing + 1) {
      for (int y = gridVertOff; y < height; y += gridHeight + gridVertSpacing + 1) {
        painter.drawRect(x, y, gridWidth, gridHeight);
      }
    }
  } else {
    for (int x = gridHorOff; x <= width; x += gridWidth) painter.drawLine(x, 0, x, height);
    for (int y = gridVertOff; y <= height; y += gridHeight) painter.drawLine(0, y, width, y);
  }
}
