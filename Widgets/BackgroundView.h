#ifndef BACKGROUNDVIEW_H
#define BACKGROUNDVIEW_H

#include "AssetView.h"
#include "Models/MessageModel.h"

#include <QObject>
#include <QWidget>

class BackgroundView : public AssetView {
  Q_OBJECT
 public:
  explicit BackgroundView(AssetScrollAreaBackground *parent);
  QSize sizeHint() const override;
  void SetResourceModel(MessageModel *_model);
  bool SetImage(QString fName);
  bool SetImage(QPixmap image);
  void WriteImage(QString fName, QString type);
  void Paint(QPainter &painter, QRect visible) override;

 private:
  MessageModel *_model;
  QPixmap _pixmap;
  QPixmap _transparentPixmap;
  QColor _transparencyColor;
};

#endif  // BACKGROUNDVIEW_H
