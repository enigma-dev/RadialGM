#ifndef ROOMVIEW_H
#define ROOMVIEW_H

#include "AssetView.h"
#include "Models/ProtoModel.h"

#include <QObject>
#include <QWidget>
#include <QSortFilterProxyModel>

class InstanceSortFilterProxyModel :  public QSortFilterProxyModel {
 public:
  InstanceSortFilterProxyModel(QWidget* parent) : QSortFilterProxyModel(parent) {}
  bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};


class RoomView : public AssetView {
  Q_OBJECT

 public:
  explicit RoomView(QWidget *parent);
  QSize sizeHint() const override;
  void SetResourceModel(ProtoModel *model);

 protected:
  void paintEvent(QPaintEvent *event) override;

 private:
  ProtoModel* model;
  InstanceSortFilterProxyModel* sortedInstances;
  QSortFilterProxyModel* sortedTiles;

  QPixmap transparentPixmap;

  void paintTiles(QPainter &painter);
  void paintBackgrounds(QPainter &painter, bool foregrounds = false);
  void paintInstances(QPainter &painter);
  void paintGrid(QPainter &painter);
};

#endif  // ROOMVIEW_H
