#ifndef ROOMRENDERER_H
#define ROOMRENDERER_H

#include "Models/ProtoModel.h"

#include <QObject>
#include <QWidget>

class RoomRenderer : public QWidget {
  Q_OBJECT

 public:
  explicit RoomRenderer(QWidget *parent);
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
};

#endif  // ROOMRENDERER_H
