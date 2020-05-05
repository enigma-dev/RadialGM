#ifndef ROOMVIEW_H
#define ROOMVIEW_H

#include "AssetView.h"
#include "Models/MessageModel.h"
#include "Models/ResourceModelMap.h"
#include "Models/RepeatedStringModel.h"
#include "Components/SpatialHash.h"

#include <QObject>
#include <QSortFilterProxyModel>
#include <QWidget>

class InstanceSortFilterProxyModel : public QSortFilterProxyModel {
 public:
  InstanceSortFilterProxyModel(QWidget *parent) : QSortFilterProxyModel(parent) {}
  bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

// TODO: Use full transform.
struct Proxy {
  int row;
  Proxy(int row): row(row) {}
  virtual int x1() const = 0;
  virtual int x2() const = 0;
  virtual int y1() const = 0;
  virtual int y2() const = 0;
  int xmin() const { return x1() < x2() ? x1() : x2(); }
  int xmax() const { return x1() > x2() ? x1() : x2(); }
  int ymin() const { return y1() < y2() ? y1() : y2(); }
  int ymax() const { return y1() > y2() ? y1() : y2(); }
  friend inline bool operator==(Proxy const& lhs, Proxy const& rhs)
  {
    return lhs.row == rhs.row;
  }
};

struct InstanceProxy : Proxy {
  InstanceSortFilterProxyModel *model;
  InstanceProxy(InstanceSortFilterProxyModel *model, int row): Proxy(row), model(model) {}
  int x1() const override { return model->data(model->index(row, Room::Instance::kXFieldNumber)).toInt(); }
  int y1() const override { return model->data(model->index(row, Room::Instance::kYFieldNumber)).toInt(); }
  int x2() const override {
    int w = sprw();
    // TODO: Resolve #122
    double xscale = model->data(model->index(row, Room::Instance::kXscaleFieldNumber)).toDouble();
    return x1() + (double)w * xscale;
  }
  int y2() const override {
    int h = sprh();
    // TODO: Resolve #122
    double yscale = model->data(model->index(row, Room::Instance::kYscaleFieldNumber)).toDouble();
    return y1() + (double)h * yscale;
  }
  int sprw() const {
    auto sprite = spr();
    if (!sprite) return 16;
    return sprite->Data(Sprite::kWidthFieldNumber).toInt();
  }
  int sprh() const {
    auto sprite = spr();
    if (!sprite) return 16;
    return sprite->Data(Sprite::kHeightFieldNumber).toInt();
  }
  MessageModel* spr() const {
    QVariant sprName = model->data(model->index(row, Room::Instance::kObjectTypeFieldNumber));
    MessageModel* spr = GetObjectSprite(sprName.toString());
    if (spr == nullptr || spr->GetSubModel<RepeatedStringModel*>(Sprite::kSubimagesFieldNumber)->Empty())
      return nullptr;
    return spr;
  }
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
  SpatialHash<InstanceProxy> _instanceHash;

  void paintTiles(QPainter &painter);
  void paintBackgrounds(QPainter &painter, bool foregrounds = false);
  void paintInstances(QPainter &painter);
};

#endif  // ROOMVIEW_H
