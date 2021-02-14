#ifndef ROOMVIEW_H
#define ROOMVIEW_H

#include "AssetView.h"
#include "Models/MessageModel.h"
#include "Models/RepeatedSortFilterProxyModel.h"

class InstanceSortFilterProxyModel : public RepeatedSortFilterProxyModel {
 public:
  InstanceSortFilterProxyModel(QObject *parent) : RepeatedSortFilterProxyModel(parent) {}
  bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
};

class RoomView : public AssetView {
  Q_OBJECT

 public:
  explicit RoomView(AssetScrollAreaBackground *parent = nullptr);
  QSize sizeHint() const override;
  void SetResourceModel(MessageModel *_model);
  void Paint(QPainter &painter) override;

 protected:
  MessageModel *_model;
  InstanceSortFilterProxyModel *_sortedInstances;
  RepeatedSortFilterProxyModel *_sortedTiles;
  QPixmap _transparentPixmap;

  void paintTiles(QPainter &painter);
  void paintBackgrounds(QPainter &painter, bool foregrounds = false);
  void paintInstances(QPainter &painter);
};

#endif  // ROOMVIEW_H
