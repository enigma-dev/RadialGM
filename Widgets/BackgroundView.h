#ifndef BACKGROUNDVIEW_H
#define BACKGROUNDVIEW_H

#include "AssetView.h"

#include <QObject>
#include <QWidget>

class BackgroundView : public AssetView {
  Q_OBJECT

  Q_PROPERTY(QPixmap Image MEMBER _pixmap NOTIFY ImageChanged USER true)

 public:
  explicit BackgroundView(AssetScrollAreaBackground *parent);
  QSize sizeHint() const override;
  bool SetImage(QString fName);
  bool SetImage(QPixmap image);
  void WriteImage(QString fName, QString type);
  void Paint(QPainter &painter) override;

 signals:
  void ImageChanged(const QPixmap &newImage);

 private:
  QPixmap _pixmap;
  QPixmap _transparentPixmap;
  QColor _transparencyColor;
};

#endif  // BACKGROUNDVIEW_H
