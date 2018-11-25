#ifndef BACKGROUNDVIEW_H
#define BACKGROUNDVIEW_H

#include "AssetView.h"
#include "Models/ProtoModel.h"

#include <QObject>
#include <QWidget>

class BackgroundView : public AssetView {
  Q_OBJECT
 public:
  explicit BackgroundView(QWidget *parent);
  QSize sizeHint() const override;
  void SetResourceModel(ProtoModel *model);
  bool SetImage(QString fName);
  bool SetImage(QPixmap image);
  void WriteImage(QString fName, QString type);

 protected:
  void paintEvent(QPaintEvent *event) override;

 private:
  ProtoModel *model;
  QPixmap pixmap;
  QPixmap transparentPixmap;
  QColor transparencyColor;
};

#endif  // BACKGROUNDVIEW_H
