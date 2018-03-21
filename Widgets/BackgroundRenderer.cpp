#include "BackgroundRenderer.h"

#include "Components/ArtManager.h"

#include <QPainter>

BackgroundRenderer::BackgroundRenderer(QWidget *parent) : QWidget(parent), zoom(1) {}

void BackgroundRenderer::setImage(QPixmap image) {
  zoom = 1;
  pixmap = image;

  QImage img = pixmap.toImage();
  img = img.convertToFormat(QImage::Format_ARGB32);
  transparencyColor = img.pixelColor(0, img.height() - 1);
  for (int x = 0; x < img.width(); ++x) {
    for (int y = 0; y < img.height(); ++y) {
      if (img.pixelColor(x, y) == transparencyColor) img.setPixelColor(x, y, Qt::transparent);
    }
  }

  transparentPixmap = QPixmap::fromImage(img);
  setFixedSize(pixmap.width() + 1, pixmap.height() + 1);
  update();
}

QSize BackgroundRenderer::sizeHint() const { return QSize(pixmap.width(), pixmap.height()); }

void BackgroundRenderer::setGrid(bool visible, int hOff, int vOff, int w, int h, int hSpacing, int vSpacing) {
  grid_visible = visible;
  grid_hOff = hOff;
  grid_vOff = vOff;
  grid_w = w;
  grid_h = h;
  grid_hSpacing = hSpacing;
  grid_vSpacing = vSpacing;
  update();
}

void BackgroundRenderer::setTransparent(bool transparent) { this->transparent = transparent; }

void BackgroundRenderer::setZoom(qreal zoom) {
  this->zoom = zoom;
  if (this->zoom > 3200) this->zoom = 3200;
  if (this->zoom < 0.0625) this->zoom = 0.0625;
  setFixedSize(static_cast<int>(pixmap.width() * this->zoom) + 1, static_cast<int>(pixmap.height() * this->zoom) + 1);
}

const qreal &BackgroundRenderer::getZoom() const { return zoom; }

void BackgroundRenderer::paintEvent(QPaintEvent * /* event */) {
  QPainter painter(this);

  painter.fillRect(QRectF(0, 0, pixmap.width() * zoom, pixmap.height() * zoom), ArtManager::get_transpareny_brush());

  painter.scale(zoom, zoom);
  painter.drawPixmap(0, 0, (transparent) ? transparentPixmap : pixmap);

  if (grid_visible) {
    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    painter.setPen(QColor(0xff, 0xff, 0xff));

    if (grid_hSpacing != 0 || grid_vSpacing != 0) {
      for (int x = grid_hOff; x < pixmap.width(); x += grid_w + grid_hSpacing) {
        for (int y = grid_vOff; y < pixmap.height(); y += grid_h + grid_vSpacing) {
          painter.drawRect(x, y, grid_w, grid_h);
        }
      }
    }

    if (grid_hSpacing == 0) {
      for (int x = grid_hOff; x <= pixmap.width(); x += grid_w) painter.drawLine(x, 0, x, pixmap.height());
    }

    if (grid_vSpacing == 0) {
      for (int y = grid_vOff; y <= pixmap.width(); y += grid_h) painter.drawLine(0, y, pixmap.width(), y);
    }
  }
}
