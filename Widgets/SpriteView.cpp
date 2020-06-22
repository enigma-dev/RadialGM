#include "SpriteView.h"
#include "Components/ArtManager.h"

#include <QGraphicsPixmapItem>

SpriteView::SpriteView(AssetScrollAreaBackground *parent) : AssetView(parent), _showBBox(true), _showOrigin(true) {
  _grid.show = false;
  parent->SetDrawSolidBackground(true, Qt::GlobalColor::transparent);
}

QSize SpriteView::sizeHint() const { return _pixmap.size(); }

void SpriteView::SetResourceModel(MessageModel *model) {
  _model = model;
  _subimgs = _model->GetSubModel<RepeatedImageModel *>(Sprite::kSubimagesFieldNumber);
  if (_subimgs->rowCount() > 0) SetSubimage(0);
}

void SpriteView::SetSubimage(int index) {
  if (index == -1) {
    _pixmap = QPixmap();
  } else if (index < -1 || index >= _subimgs->rowCount()) {
    qDebug() << "Invalid subimage index specified";
    return;
  } else {
    _pixmap = ArtManager::GetCachedPixmap(_subimgs->Data(index).toString());
  }

  _parent->update();
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
  int x = _model->Data(Sprite::kBboxLeftFieldNumber).toInt();
  int y = _model->Data(Sprite::kBboxTopFieldNumber).toInt();
  int right = _model->Data(Sprite::kBboxRightFieldNumber).toInt();
  int bottom = _model->Data(Sprite::kBboxBottomFieldNumber).toInt();
  return QRectF(x, y, right - x, bottom - y);
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
    painter.setBrush(QBrush(Qt::yellow));
    painter.drawEllipse(QPoint(_model->Data(Sprite::kOriginXFieldNumber).toInt() * zoom,
                               _model->Data(Sprite::kOriginYFieldNumber).toInt() * zoom),
                        2, 2);
  }

  painter.restore();
}
