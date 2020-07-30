#include "SpriteView.h"
#include "Components/ArtManager.h"

#include <QGraphicsPixmapItem>

SpriteView::SpriteView(AssetScrollAreaBackground *parent) : AssetView(parent), _showBBox(true), _showOrigin(true) {
  _grid.show = false;
  parent->SetDrawSolidBackground(true, Qt::GlobalColor::transparent);
}

QSize SpriteView::sizeHint() const { return _pixmap.size(); }

void SpriteView::SetSubimage(int index) {

}

void SpriteView::SetShowBBox(bool show) {
  _showBBox = show;
  _parent->update();
}

void SpriteView::SetShowOrigin(bool show) {
  _showOrigin = show;
  _parent->update();
}

QPixmap &SpriteView::GetPixmap() { return _pixmap; }

QRectF SpriteView::AutomaticBBoxRect() {
  QGraphicsPixmapItem item(_pixmap);
  return item.opaqueArea().boundingRect();
}

QRectF SpriteView::BBoxRect() {

}

void SpriteView::Paint(QPainter &painter) { painter.drawPixmap(0, 0, _pixmap); }

void SpriteView::PaintTop(QPainter &painter) {
  qreal zoom = _parent->GetZoom();

  painter.save();
  painter.translate(_parent->GetCenterOffset());

  if (_showBBox) {
    painter.save();
    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    QPen pen(Qt::white, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    painter.setPen(pen);

    painter.drawRect(
        QRectF(BBoxRect().x() * zoom, BBoxRect().y() * zoom, BBoxRect().width() * zoom, BBoxRect().height() * zoom));
    painter.restore();
  }

  if (_showOrigin) {

  }

  painter.restore();
}
