#include "RoomView.h"
#include "Components/ArtManager.h"
#include "MainWindow.h"
#include "Models/RepeatedMessageModel.h"

#include <QDebug>
#include <QPainter>

bool InstanceSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const {
  QVariant leftData = sourceModel()->data(left);
  QVariant rightData = sourceModel()->data(right);

  MessageModel* objA = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, leftData.toString());
  MessageModel* objB = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, rightData.toString());

  if (objA == nullptr || objB == nullptr) return false;

  objA = objA->GetSubModel<MessageModel*>(TreeNode::kObjectFieldNumber);
  objB = objB->GetSubModel<MessageModel*>(TreeNode::kObjectFieldNumber);

  if (objA == nullptr || objB == nullptr) return false;

  return objA->Data(Object::kDepthFieldNumber) < objB->Data(Object::kDepthFieldNumber);
}

RoomView::RoomView(AssetScrollAreaBackground* parent) : AssetView(parent), _model(nullptr) {
  setFixedSize(sizeHint());
  this->_sortedInstances = new InstanceSortFilterProxyModel(this);
  this->_sortedTiles = new QSortFilterProxyModel(this);
}

void RoomView::SetResourceModel(MessageModel* model) {
  this->_model = model;

  // TODO: Add tile hash.
  _instanceHash.clear();

  if (model != nullptr) {
    this->_sortedInstances->setSourceModel(model->GetSubModel<RepeatedMessageModel*>(Room::kInstancesFieldNumber));
    this->_sortedInstances->sort(Room::Instance::kObjectTypeFieldNumber);
    this->_sortedTiles->setSourceModel(model->GetSubModel<RepeatedMessageModel*>(Room::kTilesFieldNumber));
    this->_sortedTiles->sort(Room::Tile::kDepthFieldNumber);

    for (int row = 0; row < _sortedInstances->rowCount(); row++) {
      _instanceHash.addRectangle(InstanceProxy(_sortedInstances, row));
    }
    //TODO: Handle data changed with correct depth sorted order.
    connect(_sortedInstances, &QAbstractItemModel::modelReset, [&]() {
      _instanceHash.clear();
    });
    connect(_sortedInstances, &QAbstractItemModel::rowsInserted, [&](const QModelIndex &/*parent*/, int first, int last) {
      for (int row = first; row <= last; row++) {
        _instanceHash.addRectangle(InstanceProxy(_sortedInstances, row));
      }
    });
    connect(_sortedInstances, &QAbstractItemModel::rowsAboutToBeRemoved, [&](const QModelIndex &/*parent*/, int first, int last) {
      for (int row = first; row <= last; row++) {
        _instanceHash.removeProxy(InstanceProxy(_sortedInstances, row));
      }
    });
  }
  setFixedSize(sizeHint());
  repaint();
}

QSize RoomView::sizeHint() const {
  if (!_model) return QSize(640, 480);
  QVariant roomWidth = _model->Data(Room::kWidthFieldNumber), roomHeight = _model->Data(Room::kHeightFieldNumber);
  // TODO: add model defaults (enigma-dev/enigma-dev#1872)
  if (!roomWidth.isValid() || !roomHeight.isValid()) return QSize(640, 480);
  return QSize(roomWidth.toUInt(), roomHeight.toUInt());
}

void RoomView::Paint(QPainter& painter) {
  _grid.type = GridType::Standard;

  if (!_model) return;

  QVariant hsnap = _model->Data(Room::kHsnapFieldNumber);
  QVariant vsnap = _model->Data(Room::kVsnapFieldNumber);
  _grid.horSpacing = hsnap.isValid() ? hsnap.toInt() : 16;
  _grid.vertSpacing = vsnap.isValid() ? vsnap.toInt() : 16;

  QColor roomColor = QColor(255, 255, 255, 100);

  if (_model->Data(Room::kShowColorFieldNumber).toBool()) roomColor = _model->Data(Room::kColorFieldNumber).toInt();

  QVariant roomWidth = _model->Data(Room::kWidthFieldNumber), roomHeight = _model->Data(Room::kHeightFieldNumber);
  painter.fillRect(
      QRectF(0, 0, roomWidth.isValid() ? roomWidth.toUInt() : 640, roomWidth.isValid() ? roomHeight.toUInt() : 480),
      QBrush(roomColor));

  this->paintBackgrounds(painter, false);
  this->paintTiles(painter);
  this->paintInstances(painter);
  this->paintBackgrounds(painter, true);
}

void RoomView::paintTiles(QPainter& painter) {
  for (int row = 0; row < _sortedTiles->rowCount(); row++) {
    QVariant bkgName = _sortedTiles->data(_sortedTiles->index(row, Room::Tile::kBackgroundNameFieldNumber));
    MessageModel* bkg = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, bkgName.toString());
    if (!bkg) continue;
    bkg = bkg->GetSubModel<MessageModel*>(TreeNode::kBackgroundFieldNumber);
    if (!bkg) continue;

    MessageModel* tile = _sortedTiles->data(_sortedTiles->index(row, 0)).value<MessageModel*>();
    R_ASSESS_C(tile);
    int x = _sortedTiles->data(_sortedTiles->index(row, Room::Tile::kXFieldNumber)).toInt();
    int y = _sortedTiles->data(_sortedTiles->index(row, Room::Tile::kYFieldNumber)).toInt();
    int xOff = _sortedTiles->data(_sortedTiles->index(row, Room::Tile::kXoffsetFieldNumber)).toInt();
    int yOff = _sortedTiles->data(_sortedTiles->index(row, Room::Tile::kYoffsetFieldNumber)).toInt();
    int w = _sortedTiles->data(_sortedTiles->index(row, Room::Tile::kWidthFieldNumber)).toInt();
    int h = _sortedTiles->data(_sortedTiles->index(row, Room::Tile::kHeightFieldNumber)).toInt();

    QVariant xScale = tile->dataOrDefault(tile->index(Room::Tile::kXscaleFieldNumber));
    QVariant yScale = tile->dataOrDefault(tile->index(Room::Tile::kYscaleFieldNumber));

    QString imgFile = bkg->Data(Background::kImageFieldNumber).toString();
    QPixmap pixmap = ArtManager::GetCachedPixmap(imgFile);
    if (pixmap.isNull()) continue;

    QRectF dest(x, y, w, h);
    QRectF src(xOff, yOff, w, h);
    const QTransform transform = painter.transform();
    painter.scale(xScale.toFloat(), yScale.toFloat());
    painter.drawPixmap(dest, pixmap, src);
    painter.setTransform(transform);
  }
}

void RoomView::paintBackgrounds(QPainter& painter, bool foregrounds) {
  RepeatedMessageModel* backgrounds = _model->GetSubModel<RepeatedMessageModel*>(Room::kBackgroundsFieldNumber);
  for (int row = 0; row < backgrounds->rowCount(); row++) {
    bool visible = backgrounds->Data(row, Room::Background::kVisibleFieldNumber).toBool();
    bool foreground = backgrounds->Data(row, Room::Background::kForegroundFieldNumber).toBool();
    QString bkgName = backgrounds->Data(row, Room::Background::kBackgroundNameFieldNumber).toString();

    if (!visible || foreground != foregrounds) continue;
    MessageModel* bkgRes = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, bkgName);
    if (!bkgRes) continue;
    bkgRes = bkgRes->GetSubModel<MessageModel*>(TreeNode::kBackgroundFieldNumber);
    if (!bkgRes) continue;

    int x = backgrounds->Data(row, Room::Background::kXFieldNumber).toInt();
    int y = backgrounds->Data(row, Room::Background::kYFieldNumber).toInt();
    int w = bkgRes->Data(Background::kWidthFieldNumber).toInt();
    int h = bkgRes->Data(Background::kHeightFieldNumber).toInt();

    QString imgFile = bkgRes->Data(Background::kImageFieldNumber).toString();
    QPixmap pixmap = ArtManager::GetCachedPixmap(imgFile);
    if (pixmap.isNull()) continue;

    QRectF dest(x, y, w, h);
    QRectF src(0, 0, w, h);

    bool stretch = backgrounds->Data(row, Room::Background::kStretchFieldNumber).toBool();
    int room_w = _model->Data(Room::kWidthFieldNumber).toInt();
    int room_h = _model->Data(Room::kHeightFieldNumber).toInt();

    const QTransform transform = painter.transform();
    if (stretch) {
      painter.scale(room_w / qreal(w), room_h / qreal(h));
    }

    bool hTiled = backgrounds->Data(row, Room::Background::kHtiledFieldNumber).toBool();
    bool vTiled = backgrounds->Data(row, Room::Background::kVtiledFieldNumber).toBool();

    if (hTiled) {
      dest.setX(0);
      dest.setWidth(room_w);
      src.setX(x);
    }

    if (vTiled) {
      dest.setY(0);
      dest.setHeight(room_h);
      src.setY(y);
    }

    painter.fillRect(dest, QBrush(pixmap));
    painter.setTransform(transform);
  }
}

void RoomView::paintInstances(QPainter& painter) {
  QRectF clip = painter.clipBoundingRect();
  // TODO: Merge sort the visible instances from query window by bucket.
  auto visibleInstances = _instanceHash.queryWindow(clip.x(), clip.y(), clip.width(), clip.height());
  for (auto& proxy : visibleInstances) {
    int row = proxy.row;

    QString imgFile = ":/actions/help.png";
    int w = 16;
    int h = 16;
    int xoff = 0;
    int yoff = 0;

    QVariant sprName = _sortedInstances->data(_sortedInstances->index(row, Room::Instance::kObjectTypeFieldNumber));

    MessageModel* spr = GetObjectSprite(sprName.toString());
    if (spr == nullptr || spr->GetSubModel<RepeatedStringModel*>(Sprite::kSubimagesFieldNumber)->Empty()) {
      imgFile = "object";
    } else {
      imgFile = spr->GetSubModel<RepeatedStringModel*>(Sprite::kSubimagesFieldNumber)->Data(0).toString();
      w = spr->Data(Sprite::kWidthFieldNumber).toInt();
      h = spr->Data(Sprite::kHeightFieldNumber).toInt();
      xoff = spr->Data(Sprite::kOriginXFieldNumber).toInt();
      yoff = spr->Data(Sprite::kOriginYFieldNumber).toInt();
    }

    QPixmap pixmap = ArtManager::GetCachedPixmap(imgFile);
    if (pixmap.isNull()) continue;

    MessageModel* inst = _sortedInstances->data(_sortedInstances->index(row, 0)).value<MessageModel*>();
    R_ASSESS_C(inst);
    QVariant x = _sortedInstances->data(_sortedInstances->index(row, Room::Instance::kXFieldNumber));
    QVariant y = _sortedInstances->data(_sortedInstances->index(row, Room::Instance::kYFieldNumber));
    QVariant xScale = inst->dataOrDefault(inst->index(Room::Instance::kXscaleFieldNumber));
    QVariant yScale = inst->dataOrDefault(inst->index(Room::Instance::kYscaleFieldNumber));
    QVariant rot = inst->dataOrDefault(inst->index(Room::Instance::kRotationFieldNumber));

    QRectF dest(0, 0, w, h);
    QRectF src(0, 0, w, h);
    const QTransform transform = painter.transform();
    painter.translate(x.toInt(), y.toInt());
    painter.scale(xScale.toFloat(), yScale.toFloat());
    painter.rotate(rot.toFloat());
    painter.translate(-xoff, -yoff);
    painter.drawPixmap(dest, pixmap, src);
    painter.setTransform(transform);
  }
}
