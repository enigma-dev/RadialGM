#include "AssetView.h"

AssetView::AssetView(QWidget* parent) : QWidget(parent), zoom(1) {}

const qreal& AssetView::GetZoom() const { return zoom; }

void AssetView::paintGrid(QPainter& painter, int width, int height, int gridHorSpacing, int gridVertSpacing,
                          int gridHorOff, int gridVertOff, int gridWidth, int gridHeight) {
  painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
  painter.setPen(QColor(0xff, 0xff, 0xff));

  if (gridHorSpacing != 0 || gridVertSpacing != 0) {
    for (int x = gridHorOff; x < width; x += gridWidth + gridHorSpacing) {
      for (int y = gridVertOff; y < height; y += gridHeight + gridVertSpacing) {
        painter.drawRect(x, y, gridWidth, gridHeight);
      }
    }
  }

  if (gridHorSpacing == 0) {
    for (int x = gridHorOff; x <= width; x += gridWidth) painter.drawLine(x, 0, x, height);
  }

  if (gridVertSpacing == 0) {
    for (int y = gridVertOff; y <= height; y += gridHeight) painter.drawLine(0, y, width, y);
  }
}
