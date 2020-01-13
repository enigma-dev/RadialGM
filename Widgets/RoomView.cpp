#include "RoomView.h"
#include "Components/ArtManager.h"
#include "MainWindow.h"

#include <QDebug>
#include <QPainter>

bool InstanceSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const {
  QVariant leftData = sourceModel()->data(left);
  QVariant rightData = sourceModel()->data(right);

  ProtoModelPtr objA = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, leftData.toString());
  ProtoModelPtr objB = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, rightData.toString());

  if (objA == nullptr || objB == nullptr) return false;

  objA = objA->GetSubModel(TreeNode::kObjectFieldNumber);
  objB = objB->GetSubModel(TreeNode::kObjectFieldNumber);

  if (objA == nullptr || objB == nullptr) return false;

  return objA->data(Object::kDepthFieldNumber) < objB->data(Object::kDepthFieldNumber);
}

RoomView::RoomView(AssetScrollAreaBackground* parent) : AssetView(parent), model(nullptr) {
  setFixedSize(sizeHint());
  this->sortedInstances = new InstanceSortFilterProxyModel(this);
  this->sortedTiles = new QSortFilterProxyModel(this);
}

void RoomView::SetResourceModel(ProtoModelPtr model) {
  this->model = model;

  if (model != nullptr) {
    this->sortedInstances->setSourceModel(model->GetRepeatedSubModel(Room::kInstancesFieldNumber));
    this->sortedInstances->sort(Room::Instance::kObjectTypeFieldNumber);
    this->sortedTiles->setSourceModel(model->GetRepeatedSubModel(Room::kTilesFieldNumber));
    this->sortedTiles->sort(Room::Tile::kDepthFieldNumber);
  }
  setFixedSize(sizeHint());
  repaint();
}

QSize RoomView::sizeHint() const {
  if (!model) return QSize(640, 480);
  QVariant roomWidth = model->data(Room::kWidthFieldNumber), roomHeight = model->data(Room::kHeightFieldNumber);
  // TODO: add model defaults (enigma-dev/enigma-dev#1872)
  if (!roomWidth.isValid() || !roomHeight.isValid()) return QSize(640, 480);
  return QSize(roomWidth.toUInt(), roomHeight.toUInt());
}

void RoomView::Paint(QPainter& painter) {
  grid.type = GridType::Standard;

  if (!model) return;

  QVariant hsnap = model->data(Room::kHsnapFieldNumber);
  QVariant vsnap = model->data(Room::kVsnapFieldNumber);
  grid.horSpacing = hsnap.isValid() ? hsnap.toInt() : 16;
  grid.vertSpacing = vsnap.isValid() ? vsnap.toInt() : 16;

  QColor roomColor = QColor(255, 255, 255, 100);

  if (model->data(Room::kShowColorFieldNumber).toBool()) roomColor = model->data(Room::kColorFieldNumber).toInt();

  QVariant roomWidth = model->data(Room::kWidthFieldNumber), roomHeight = model->data(Room::kHeightFieldNumber);
  painter.fillRect(
      QRectF(0, 0, roomWidth.isValid() ? roomWidth.toUInt() : 640, roomWidth.isValid() ? roomHeight.toUInt() : 480),
      QBrush(roomColor));

  this->paintBackgrounds(painter, false);
  this->paintTiles(painter);
  this->paintInstances(painter);
  this->paintBackgrounds(painter, true);
}

void RoomView::paintTiles(QPainter& painter) {
  for (int row = 0; row < sortedTiles->rowCount(); row++) {
    QVariant bkgName = sortedTiles->data(sortedTiles->index(row, Room::Tile::kBackgroundNameFieldNumber));
    ProtoModelPtr bkg = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, bkgName.toString());
    if (!bkg) continue;
    bkg = bkg->GetSubModel(TreeNode::kBackgroundFieldNumber);
    if (!bkg) continue;

    int x = sortedTiles->data(sortedTiles->index(row, Room::Tile::kXFieldNumber)).toInt();
    int y = sortedTiles->data(sortedTiles->index(row, Room::Tile::kYFieldNumber)).toInt();
    int xOff = sortedTiles->data(sortedTiles->index(row, Room::Tile::kXoffsetFieldNumber)).toInt();
    int yOff = sortedTiles->data(sortedTiles->index(row, Room::Tile::kYoffsetFieldNumber)).toInt();
    int w = sortedTiles->data(sortedTiles->index(row, Room::Tile::kWidthFieldNumber)).toInt();
    int h = sortedTiles->data(sortedTiles->index(row, Room::Tile::kHeightFieldNumber)).toInt();

    QVariant xScale = sortedTiles->data(sortedTiles->index(row, Room::Tile::kXscaleFieldNumber));
    QVariant yScale = sortedTiles->data(sortedTiles->index(row, Room::Tile::kYscaleFieldNumber));

    if (xScale.isNull()) xScale.setValue(1);
    if (yScale.isNull()) yScale.setValue(1);

    QString imgFile = bkg->data(Background::kImageFieldNumber).toString();
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
  RepeatedProtoModelPtr backgrounds = model->GetRepeatedSubModel(Room::kBackgroundsFieldNumber);
  for (int row = 0; row < backgrounds->rowCount(); row++) {
    bool visible = backgrounds->data(row, Room::Background::kVisibleFieldNumber).toBool();
    bool foreground = backgrounds->data(row, Room::Background::kForegroundFieldNumber).toBool();
    QString bkgName = backgrounds->data(row, Room::Background::kBackgroundNameFieldNumber).toString();

    if (!visible || foreground != foregrounds) continue;
    ProtoModelPtr bkgRes = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, bkgName);
    if (!bkgRes) continue;
    bkgRes = bkgRes->GetSubModel(TreeNode::kBackgroundFieldNumber);
    if (!bkgRes) continue;

    int x = backgrounds->data(row, Room::Background::kXFieldNumber).toInt();
    int y = backgrounds->data(row, Room::Background::kYFieldNumber).toInt();
    int w = bkgRes->data(Background::kWidthFieldNumber).toInt();
    int h = bkgRes->data(Background::kHeightFieldNumber).toInt();

    QString imgFile = bkgRes->data(Background::kImageFieldNumber).toString();
    QPixmap pixmap = ArtManager::GetCachedPixmap(imgFile);
    if (pixmap.isNull()) continue;

    QRectF dest(x, y, w, h);
    QRectF src(0, 0, w, h);

    bool stretch = backgrounds->data(row, Room::Background::kStretchFieldNumber).toBool();
    int room_w = model->data(Room::kWidthFieldNumber).toInt();
    int room_h = model->data(Room::kHeightFieldNumber).toInt();

    const QTransform transform = painter.transform();
    if (stretch) {
      painter.scale(room_w / qreal(w), room_h / qreal(h));
    }

    bool hTiled = backgrounds->data(row, Room::Background::kHtiledFieldNumber).toBool();
    bool vTiled = backgrounds->data(row, Room::Background::kVtiledFieldNumber).toBool();

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
  for (int row = 0; row < sortedInstances->rowCount(); row++) {
    QString imgFile = ":/actions/help.png";
    int w = 16;
    int h = 16;
    int xoff = 0;
    int yoff = 0;

    QVariant sprName = sortedInstances->data(sortedInstances->index(row, Room::Instance::kObjectTypeFieldNumber));

    const ProtoModelPtr spr = GetObjectSprite(sprName.toString());
    if (spr == nullptr)
      imgFile = "object";
    else {
      imgFile = spr->GetRepeatedStringSubModel(Sprite::kSubimagesFieldNumber)->data(0).toString();
      w = spr->data(Sprite::kWidthFieldNumber).toInt();
      h = spr->data(Sprite::kHeightFieldNumber).toInt();
      xoff = spr->data(Sprite::kOriginXFieldNumber).toInt();
      yoff = spr->data(Sprite::kOriginYFieldNumber).toInt();
    }

    QPixmap pixmap = ArtManager::GetCachedPixmap(imgFile);
    if (pixmap.isNull()) continue;

    QVariant x = sortedInstances->data(sortedInstances->index(row, Room::Instance::kXFieldNumber));
    QVariant y = sortedInstances->data(sortedInstances->index(row, Room::Instance::kYFieldNumber));
    QVariant xScale = sortedInstances->data(sortedInstances->index(row, Room::Instance::kXscaleFieldNumber));
    QVariant yScale = sortedInstances->data(sortedInstances->index(row, Room::Instance::kYscaleFieldNumber));
    QVariant rot = sortedInstances->data(sortedInstances->index(row, Room::Instance::kRotationFieldNumber));

    QRectF dest(0, 0, w, h);
    QRectF src(0, 0, w, h);
    const QTransform transform = painter.transform();
    painter.translate(x.toInt(), y.toInt());
    painter.scale(xScale.isValid() ? xScale.toFloat() : 1, yScale.isValid() ? yScale.toFloat() : 1);
    painter.rotate(rot.toFloat());
    painter.translate(-xoff, -yoff);
    painter.drawPixmap(dest, pixmap, src);
    painter.setTransform(transform);
  }
}
