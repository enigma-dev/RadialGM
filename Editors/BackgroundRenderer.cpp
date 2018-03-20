#include "BackgroundRenderer.h"
#include "../ArtManager.h"

#include <QPainter>

BackgroundRenderer::BackgroundRenderer(QWidget *parent) : QWidget(parent) {
}

void BackgroundRenderer::setImage(QString imageFile) {
    image = QPixmap(imageFile);
    setFixedSize(image.width(), image.height());
    update();
}

QSize BackgroundRenderer::sizeHint() const
{
    return QSize(image.width(), image.height());
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

void BackgroundRenderer::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.fillRect(QRectF(0, 0, image.width(), image.height()), ArtManager::get_transpareny_brush());

    painter.drawPixmap(0, 0, image);


    if (grid_visible) {
        painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
        painter.setPen(QColor(0xff, 0xff, 0xff));

        for (int x = grid_hOff; x < image.width(); x+= grid_w + grid_hSpacing) {
            for (int y = grid_vOff; y < image.height(); y+= grid_h + grid_vSpacing) {
                painter.drawRect(x, y, grid_w-1, grid_h-1);
            }
        }
    }
}
