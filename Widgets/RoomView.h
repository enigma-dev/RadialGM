#ifndef ROOMVIEW_H
#define ROOMVIEW_H

#include "AssetView.h"

#include <QObject>
#include <QSortFilterProxyModel>
#include <QWidget>

class InstanceSortFilterProxyModel : public QSortFilterProxyModel {
 public:
  InstanceSortFilterProxyModel(QWidget *parent) : QSortFilterProxyModel(parent) {}
  bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

class RoomView : public AssetView {
  Q_OBJECT

 public:
  explicit RoomView(AssetScrollAreaBackground *parent = nullptr);
  QSize sizeHint() const override;
  void Paint(QPainter &painter) override;

 protected:
  InstanceSortFilterProxyModel *_sortedInstances;
  QSortFilterProxyModel *_sortedTiles;
  QPixmap _transparentPixmap;

  void paintTiles(QPainter &painter);
  void paintBackgrounds(QPainter &painter, bool foregrounds = false);
  void paintInstances(QPainter &painter);
};

#endif  // ROOMVIEW_H
