#ifndef SPRITEVIEW_H
#define SPRITEVIEW_H

#include "AssetView.h"
#include "Models/ProtoModel.h"
#include "Models/RepeatedImageModel.h"

#include <QObject>
#include <QWidget>

class SpriteView : public AssetView {
  Q_OBJECT
 public:
  SpriteView(AssetScrollAreaBackground *parent);
  QSize sizeHint() const override;
  void SetResourceModel(ProtoModelPtr model);
  void Paint(QPainter &painter) override;
  void PaintTop(QPainter &painter) override;
  QRectF AutomaticBBoxRect();
  QPixmap &GetPixmap();
  QRectF BBoxRect();

 public slots:
  void SetSubimage(int index);
  void SetShowBBox(bool show);
  void SetShowOrigin(bool show);

 private:
  ProtoModelPtr _model;
  QPixmap _pixmap;
  RepeatedModel<RepeatedImageModel *> *_subimgs;
  bool _showBBox;
  bool _showOrigin;
};

#endif  // SPRITEVIEW_H
