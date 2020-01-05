#ifndef BACKGROUNDVIEW_H
#define BACKGROUNDVIEW_H

#include "AssetView.h"
#include "Models/ProtoModel.h"

#include <QObject>
#include <QWidget>

class BackgroundView : public AssetView {
  Q_OBJECT
 public:
  explicit BackgroundView(AssetScrollAreaBackground *parent);
  QSize sizeHint() const override;
  void SetResourceModel(ProtoModelPtr model);
  bool SetImage(QString fName);
  bool SetImage(QPixmap image);
  void WriteImage(QString fName, QString type);
  void Paint(QPainter &painter) override;

 private:
  ProtoModelPtr model;
  QPixmap pixmap;
  QPixmap transparentPixmap;
  QColor transparencyColor;
};

#endif  // BACKGROUNDVIEW_H
