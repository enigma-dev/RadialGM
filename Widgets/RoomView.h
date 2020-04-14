#ifndef ROOMVIEW_H
#define ROOMVIEW_H

#include "AssetView.h"
#include "Models/MessageModel.h"
#include "Components/SpatialHash.h"

#include <QObject>
#include <QSortFilterProxyModel>
#include <QWidget>

class InstanceSortFilterProxyModel : public QSortFilterProxyModel {
 public:
  InstanceSortFilterProxyModel(QWidget *parent) : QSortFilterProxyModel(parent) {}
  bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

struct Proxy {
  virtual int x1() const = 0;
  virtual int x2() const = 0;
  virtual int y1() const = 0;
  virtual int y2() const = 0;
  int xmin() const { return x1() < x2() ? x1() : x2(); }
  int xmax() const { return x1() > x2() ? x1() : x2(); }
  int ymin() const { return y1() < y2() ? y1() : y2(); }
  int ymax() const { return y1() > y2() ? y1() : y2(); }
};

struct InstanceProxy : Proxy {
  MessageModel* model;
  InstanceProxy(MessageModel* model): model(model) {}
  int x1() const override { return model->Data(Room::Instance::kXFieldNumber, 0).toInt(); }
  int y1() const override { return model->Data(Room::Instance::kYFieldNumber, 0).toInt(); }
  int x2() const override { return model->Data(Room::Instance::kXscaleFieldNumber, 0).toInt(); }
  int y2() const override { return model->Data(Room::Instance::kYscaleFieldNumber, 0).toInt(); }
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
  QSortFilterProxyModel *_sortedTiles;
  QPixmap _transparentPixmap;
  SpatialHash<InstanceProxy> instanceHash;

  void paintTiles(QPainter &painter);
  void paintBackgrounds(QPainter &painter, bool foregrounds = false);
  void paintInstances(QPainter &painter);
};

#endif  // ROOMVIEW_H
