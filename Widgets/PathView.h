#ifndef PATHVIEW_H
#define PATHVIEW_H

#include "RoomView.h"

class PathView : public RoomView {
  Q_OBJECT

 public:
  explicit PathView(AssetScrollAreaBackground *parent = nullptr);
  void Paint(QPainter &painter) override;
  void SetPathModel(ProtoModelPtr model);

  QPoint mousePos;

 protected:
  ProtoModelPtr pathModel;
};

#endif  // PATHVIEW_H
