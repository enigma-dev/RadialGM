#include "BackgroundRenderer.h"
#include "../ArtManager.h"

#include <QPainter>

BackgroundRenderer::BackgroundRenderer(QWidget *parent) : QWidget(parent), zoom(1) {
}

void BackgroundRenderer::setImage(QPixmap image) {
    pixmap = image;

    QImage img = pixmap.toImage();
    img = img.convertToFormat(QImage::Format_ARGB32);
    transparencyColor = img.pixelColor(0, img.height()-1);
    for (int x = 0; x < img.width(); ++x) {
        for (int y = 0; y < img.height(); ++y) {
            if (img.pixelColor(x, y) == transparencyColor)
                img.setPixelColor(x, y, Qt::transparent);
        }
    }

    transparentPixmap = QPixmap::fromImage(img);
    setFixedSize(pixmap.width(), pixmap.height());
    update();
}


QSize BackgroundRenderer::sizeHint() const
{
    return QSize(pixmap.width(), pixmap.height());
}

void BackgroundRenderer::setGrid(bool visible, unsigned hOff, unsigned vOff, unsigned w, unsigned h, unsigned hSpacing, unsigned vSpacing) {
    grid_visible = visible;
    grid_hOff = hOff;
    grid_vOff = vOff;
    grid_w = w;
    grid_h = h;
    grid_hSpacing = hSpacing;
    grid_vSpacing = vSpacing;
    update();
}

void BackgroundRenderer::setTransparent(bool transparent) {
    this->transparent = transparent;
}

void BackgroundRenderer::setZoom(qreal zoom) {
    this->zoom = zoom;
    if (this->zoom > 3200) this->zoom = 3200;
    if (this->zoom < 0.0625) this->zoom = 0.0625;
    setFixedSize(pixmap.width()*this->zoom, pixmap.height()*this->zoom);
}

const qreal& BackgroundRenderer::getZoom() const {
    return zoom;
}


void BackgroundRenderer::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.scale(zoom, zoom);

    painter.fillRect(QRectF(0, 0, pixmap.width(), pixmap.height()), ArtManager::get_transpareny_brush());

    painter.drawPixmap(0, 0, (transparent) ? transparentPixmap : pixmap);

    if (grid_visible) {
        painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
        painter.setPen(QColor(0xff, 0xff, 0xff));

        for (int x = grid_hOff; x < pixmap.width(); x+= grid_w + grid_hSpacing) {
            for (int y = grid_vOff; y < pixmap.height(); y+= grid_h + grid_vSpacing) {
                painter.drawRect(x, y, grid_w-1, grid_h-1);
            }
        }
    }
}
