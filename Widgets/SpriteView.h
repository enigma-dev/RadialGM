#ifndef SPRITEVIEW_H
#define SPRITEVIEW_H

#include "AssetView.h"
#include "Models/MessageModel.h"
#include "Models/RepeatedImageModel.h"

#include <QObject>
#include <QWidget>

class SpriteView : public AssetView {
  Q_OBJECT
 public:
  SpriteView(AssetScrollAreaBackground *parent);
  QSize sizeHint() const override;
  void SetResourceModel(MessageModel *model);
  void Paint(QPainter &painter, QRect visible) override;
  void PaintTop(QPainter &painter) override;
  QRectF AutomaticBBoxRect();
  QPixmap &GetPixmap();
  QRectF BBoxRect();

 public slots:
  void SetSubimage(int index);
  void SetShowBBox(bool show);
  void SetShowOrigin(bool show);

 private:
  MessageModel *_model;
  QPixmap _pixmap;
  RepeatedImageModel *_subimgs;
  bool _showBBox;
  bool _showOrigin;
};

#endif  // SPRITEVIEW_H
