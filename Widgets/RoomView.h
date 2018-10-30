#ifndef ROOMVIEW_H
#define ROOMVIEW_H

#include "AssetView.h"
#include "Models/ProtoModel.h"

#include <QObject>
#include <QWidget>

class RoomView : public AssetView {
  Q_OBJECT

 public:
  explicit RoomView(QWidget *parent);
  QSize sizeHint() const override;
  void SetResourceModel(ProtoModel *model);
  void SetZoom(qreal zoom);
  const qreal &GetZoom() const;

 protected:
  void paintEvent(QPaintEvent *event) override;

 private:
  ProtoModel *model;
  QPixmap transparentPixmap;
  qreal zoom;

  void paintTiles(QPainter &painter, Room *room);
  void paintBackgrounds(QPainter &painter, Room *room, bool foregrounds = false);
  void paintInstances(QPainter &painter, Room *room);
  void paintGrid(QPainter &painter, Room *room);
};

#endif  // ROOMVIEW_H
