#ifndef BACKGROUNDRENDERER_H
#define BACKGROUNDRENDERER_H

#include "Models/ProtoModel.h"

#include <QObject>
#include <QWidget>

class BackgroundRenderer : public QWidget {
  Q_OBJECT
 public:
  explicit BackgroundRenderer(QWidget *parent);
  QSize sizeHint() const override;
  void SetResourceModel(ResourceModel *model);
  bool SetImage(QString fName);
  bool SetImage(QPixmap image);
  void WriteImage(QString fName, QString type);
  void SetZoom(qreal zoom);
  const qreal &GetZoom() const;

 protected:
  void paintEvent(QPaintEvent *event) override;

 private:
  ResourceModel *model;
  QPixmap pixmap;
  QPixmap transparentPixmap;
  QColor transparencyColor;
  qreal zoom;
};

#endif  // BACKGROUNDRENDERER_H
