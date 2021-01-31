#include "RoomView.h"
#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "MainWindow.h"
#include "Models/MessageModel.h"
#include "Models/RepeatedMessageModel.h"
#include "Models/RepeatedModel.h"

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

  return objA->Data(FieldPath::Of<Object>(Object::kDepthFieldNumber)).toInt() <
         objB->Data(FieldPath::Of<Object>(Object::kDepthFieldNumber)).toInt();
}

RoomView::RoomView(AssetScrollAreaBackground* parent) : AssetView(parent), _model(nullptr) {
  setFixedSize(sizeHint());
  this->_sortedInstances = new InstanceSortFilterProxyModel(this);
  this->_sortedTiles = new QSortFilterProxyModel(this);
}

void RoomView::SetResourceModel(MessageModel* model) {
  this->_model = model;

  if (model != nullptr) {
    this->_sortedInstances->setSourceModel(model->GetSubModel<RepeatedMessageModel*>(Room::kInstancesFieldNumber));
    this->_sortedInstances->sort(Room::Instance::kObjectTypeFieldNumber);
    this->_sortedTiles->setSourceModel(model->GetSubModel<RepeatedMessageModel*>(Room::kTilesFieldNumber));
    this->_sortedTiles->sort(Room::Tile::kDepthFieldNumber);
  }
  setFixedSize(sizeHint());
  repaint();
}

QSize RoomView::sizeHint() const {
  if (!_model) return QSize(640, 480);
  QVariant roomWidth = _model->Data(FieldPath::Of<Room>(Room::kWidthFieldNumber)),
           roomHeight = _model->Data(FieldPath::Of<Room>(Room::kHeightFieldNumber));
  // TODO: add model defaults (enigma-dev/enigma-dev#1872)
  if (!roomWidth.isValid() || !roomHeight.isValid()) return QSize(640, 480);
  return QSize(roomWidth.toUInt(), roomHeight.toUInt());
}

void RoomView::Paint(QPainter& painter) {
  _grid.type = GridType::Standard;

  if (!_model) return;

  QVariant hsnap = _model->Data(FieldPath::Of<Room>(Room::kHsnapFieldNumber));
  QVariant vsnap = _model->Data(FieldPath::Of<Room>(Room::kVsnapFieldNumber));
  _grid.horSpacing = hsnap.isValid() ? hsnap.toInt() : 16;
  _grid.vertSpacing = vsnap.isValid() ? vsnap.toInt() : 16;

  QColor roomColor = QColor(255, 255, 255, 100);

  if (_model->Data(FieldPath::Of<Room>(Room::kShowColorFieldNumber)).toBool())
    roomColor = _model->Data(FieldPath::Of<Room>(Room::kColorFieldNumber)).toInt();

  QVariant roomWidth = _model->Data(FieldPath::Of<Room>(Room::kWidthFieldNumber)),
           roomHeight = _model->Data(FieldPath::Of<Room>(Room::kHeightFieldNumber));
  painter.fillRect(
      QRectF(0, 0, roomWidth.isValid() ? roomWidth.toUInt() : 640, roomWidth.isValid() ? roomHeight.toUInt() : 480),
      QBrush(roomColor));

  this->paintBackgrounds(painter, false);
  this->paintTiles(painter);
  this->paintInstances(painter);
  this->paintBackgrounds(painter, true);
}

void RoomView::paintTiles(QPainter& painter) {
  //FIXME: data() -> Data()
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

    QString imgFile = bkg->Data(FieldPath::Of<Background>(Background::kImageFieldNumber)).toString();
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
    bool visible = backgrounds
                       ->Data(FieldPath::Of<Room::Background>(
                           FieldPath::RepeatedOffset(Room::Background::kVisibleFieldNumber, row)))
                       .toBool();
    bool foreground = backgrounds
                          ->Data(FieldPath::Of<Room::Background>(
                              FieldPath::RepeatedOffset(Room::Background::kForegroundFieldNumber, row)))
                          .toBool();
    QString bkgName = backgrounds
                          ->Data(FieldPath::Of<Room::Background>(
                              FieldPath::RepeatedOffset(Room::Background::kBackgroundNameFieldNumber, row)))
                          .toString();

    if (!visible || foreground != foregrounds) continue;
    MessageModel* bkgRes = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, bkgName);
    if (!bkgRes) continue;
    bkgRes = bkgRes->GetSubModel<MessageModel*>(TreeNode::kBackgroundFieldNumber);
    if (!bkgRes) continue;

    int x = backgrounds
                ->Data(FieldPath::Of<Room::Background>(FieldPath::RepeatedOffset(Room::Background::kXFieldNumber, row)))
                .toInt();
    int y = backgrounds
                ->Data(FieldPath::Of<Room::Background>(FieldPath::RepeatedOffset(Room::Background::kYFieldNumber, row)))
                .toInt();
    int w = bkgRes->Data(FieldPath::Of<Background>(Background::kWidthFieldNumber)).toInt();
    int h = bkgRes->Data(FieldPath::Of<Background>(Background::kHeightFieldNumber)).toInt();

    QString imgFile = bkgRes->Data(FieldPath::Of<Background>(Background::kImageFieldNumber)).toString();
    QPixmap pixmap = ArtManager::GetCachedPixmap(imgFile);
    if (pixmap.isNull()) continue;

    QRectF dest(x, y, w, h);
    QRectF src(0, 0, w, h);

    bool stretch = backgrounds
                       ->Data(FieldPath::Of<Room::Background>(
                           FieldPath::RepeatedOffset(Room::Background::kStretchFieldNumber, row)))
                       .toBool();
    int room_w = _model->Data(FieldPath::Of<Room>(Room::kWidthFieldNumber)).toInt();
    int room_h = _model->Data(FieldPath::Of<Room>(Room::kHeightFieldNumber)).toInt();

    const QTransform transform = painter.transform();
    if (stretch) {
      painter.scale(room_w / qreal(w), room_h / qreal(h));
    }

    bool hTiled = backgrounds
                      ->Data(FieldPath::Of<Room::Background>(
                          FieldPath::RepeatedOffset(Room::Background::kHtiledFieldNumber, row)))
                      .toBool();
    bool vTiled = backgrounds
                      ->Data(FieldPath::Of<Room::Background>(
                          FieldPath::RepeatedOffset(Room::Background::kVtiledFieldNumber, row)))
                      .toBool();

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
  for (int row = 0; row < _sortedInstances->rowCount(); row++) {
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
      imgFile =
          spr->Data(FieldPath::Of<Sprite>(FieldPath::RepeatedOffset(Sprite::kSubimagesFieldNumber, 0))).toString();
      w = spr->Data(FieldPath::Of<Sprite>(Sprite::kWidthFieldNumber)).toInt();
      h = spr->Data(FieldPath::Of<Sprite>(Sprite::kHeightFieldNumber)).toInt();
      xoff = spr->Data(FieldPath::Of<Sprite>(Sprite::kOriginXFieldNumber)).toInt();
      yoff = spr->Data(FieldPath::Of<Sprite>(Sprite::kOriginYFieldNumber)).toInt();
    }

    QPixmap pixmap = ArtManager::GetCachedPixmap(imgFile);
    if (pixmap.isNull()) continue;

    MessageModel* inst = _sortedInstances->data(_sortedInstances->index(row, 0)).value<MessageModel*>();
    R_ASSESS_C(inst);
    //FIXME: change these to use Data()
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
