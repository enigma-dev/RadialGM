#ifndef BACKGROUNDRENDERER_H
#define BACKGROUNDRENDERER_H

#include "AssetView.h"
#include "Models/ProtoModel.h"

#include <QObject>
#include <QWidget>

class BackgroundRenderer : public AssetView {
  Q_OBJECT
 public:
  explicit BackgroundRenderer(QWidget *parent);
  QSize sizeHint() const override;
  void SetResourceModel(ProtoModel *model);
  bool SetImage(QString fName);
  bool SetImage(QPixmap image);
  void WriteImage(QString fName, QString type);
  void SetZoom(qreal zoom);
  const qreal &GetZoom() const;

 protected:
  void paintEvent(QPaintEvent *event) override;

 private:
  ProtoModel *model;
  QPixmap pixmap;
  QPixmap transparentPixmap;
  QColor transparencyColor;
  qreal zoom;
};

#endif  // BACKGROUNDRENDERER_H
