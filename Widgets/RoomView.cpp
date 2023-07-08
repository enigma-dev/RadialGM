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
  setFixedSize(RoomView::sizeHint());
  _sortedInstances = new InstanceSortFilterProxyModel(this);
  _sortedTiles = new RepeatedSortFilterProxyModel(this);
}

void RoomView::SetResourceModel(MessageModel* model) {
  _model = model;

  if (model != nullptr) {
    _sortedInstances->SetSourceModel(model->GetSubModel<RepeatedMessageModel*>(Room::kInstancesFieldNumber));
    _sortedInstances->sort(Room::Instance::kObjectTypeFieldNumber);
    _sortedTiles->SetSourceModel(model->GetSubModel<RepeatedMessageModel*>(Room::kTilesFieldNumber));
    _sortedTiles->sort(Room::Tile::kDepthFieldNumber);
  }
  setFixedSize(sizeHint());
  repaint();
}

QSize RoomView::sizeHint() const {
  if (!_model) return QSize(640, 480);
  QVariant roomWidth = _model->DataOrDefault(FieldPath::Of<Room>(Room::kWidthFieldNumber), 640),
           roomHeight = _model->DataOrDefault(FieldPath::Of<Room>(Room::kHeightFieldNumber), 480);
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

  paintBackgrounds(painter, false);
  paintTiles(painter);
  paintInstances(painter);
  paintBackgrounds(painter, true);
}

void RoomView::paintTiles(QPainter& painter) {
  for (int row = 0; row < _sortedTiles->rowCount(); row++) {
    QVariant bkgName = _sortedTiles->Data(
        FieldPath::Of<Room::Tile>(FieldPath::StartingAt(row), Room::Tile::kBackgroundNameFieldNumber));
    MessageModel* bkg = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, bkgName.toString());
    if (!bkg) continue;
    bkg = bkg->GetSubModel<MessageModel*>(TreeNode::kBackgroundFieldNumber);
    if (!bkg) continue;

    int x =
        _sortedTiles->Data(FieldPath::Of<Room::Tile>(FieldPath::StartingAt(row), Room::Tile::kXFieldNumber)).toInt();
    int y =
        _sortedTiles->Data(FieldPath::Of<Room::Tile>(FieldPath::StartingAt(row), Room::Tile::kYFieldNumber)).toInt();
    int xOff =
        _sortedTiles->Data(FieldPath::Of<Room::Tile>(FieldPath::StartingAt(row), Room::Tile::kXoffsetFieldNumber))
            .toInt();
    int yOff =
        _sortedTiles->Data(FieldPath::Of<Room::Tile>(FieldPath::StartingAt(row), Room::Tile::kYoffsetFieldNumber))
            .toInt();
    int w = _sortedTiles->Data(FieldPath::Of<Room::Tile>(FieldPath::StartingAt(row), Room::Tile::kWidthFieldNumber))
                .toInt();
    int h = _sortedTiles->Data(FieldPath::Of<Room::Tile>(FieldPath::StartingAt(row), Room::Tile::kHeightFieldNumber))
                .toInt();

    QVariant xScale = _sortedTiles->DataOrDefault(
        FieldPath::Of<Room::Tile>(FieldPath::StartingAt(row), Room::Tile::kXscaleFieldNumber));
    QVariant yScale = _sortedTiles->DataOrDefault(
        FieldPath::Of<Room::Tile>(FieldPath::StartingAt(row), Room::Tile::kYscaleFieldNumber));

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
    bool visible =
        backgrounds
            ->Data(FieldPath::Of<Room::Background>(FieldPath::StartingAt(row), Room::Background::kVisibleFieldNumber))
            .toBool();
    bool foreground = backgrounds
                          ->Data(FieldPath::Of<Room::Background>(FieldPath::StartingAt(row),
                                                                 Room::Background::kForegroundFieldNumber))
                          .toBool();
    QString bkgName = backgrounds
                          ->Data(FieldPath::Of<Room::Background>(FieldPath::StartingAt(row),
                                                                 Room::Background::kBackgroundNameFieldNumber))
                          .toString();

    if (!visible || foreground != foregrounds) continue;
    MessageModel* bkgRes = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, bkgName);
    if (!bkgRes) continue;
    bkgRes = bkgRes->GetSubModel<MessageModel*>(TreeNode::kBackgroundFieldNumber);
    if (!bkgRes) continue;

    int x =
        backgrounds->Data(FieldPath::Of<Room::Background>(FieldPath::StartingAt(row), Room::Background::kXFieldNumber))
            .toInt();
    int y =
        backgrounds->Data(FieldPath::Of<Room::Background>(FieldPath::StartingAt(row), Room::Background::kYFieldNumber))
            .toInt();
    int w = bkgRes->Data(FieldPath::Of<Background>(Background::kWidthFieldNumber)).toInt();
    int h = bkgRes->Data(FieldPath::Of<Background>(Background::kHeightFieldNumber)).toInt();

    QString imgFile = bkgRes->Data(FieldPath::Of<Background>(Background::kImageFieldNumber)).toString();
    QPixmap pixmap = ArtManager::GetCachedPixmap(imgFile);
    if (pixmap.isNull()) continue;

    QRectF dest(x, y, w, h);
    QRectF src(0, 0, w, h);

    bool stretch =
        backgrounds
            ->Data(FieldPath::Of<Room::Background>(FieldPath::StartingAt(row), Room::Background::kStretchFieldNumber))
            .toBool();
    int room_w = _model->Data(FieldPath::Of<Room>(Room::kWidthFieldNumber)).toInt();
    int room_h = _model->Data(FieldPath::Of<Room>(Room::kHeightFieldNumber)).toInt();

    const QTransform transform = painter.transform();
    if (stretch) {
      painter.scale(room_w / qreal(w), room_h / qreal(h));
    }

    bool hTiled =
        backgrounds
            ->Data(FieldPath::Of<Room::Background>(FieldPath::StartingAt(row), Room::Background::kHtiledFieldNumber))
            .toBool();
    bool vTiled =
        backgrounds
            ->Data(FieldPath::Of<Room::Background>(FieldPath::StartingAt(row), Room::Background::kVtiledFieldNumber))
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

    QVariant sprName = _sortedInstances->Data(
        FieldPath::Of<Room::Instance>(FieldPath::StartingAt(row), Room::Instance::kObjectTypeFieldNumber));

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

    QVariant x = _sortedInstances->Data(
        FieldPath::Of<Room::Instance>(FieldPath::StartingAt(row), Room::Instance::kXFieldNumber));
    QVariant y = _sortedInstances->Data(
        FieldPath::Of<Room::Instance>(FieldPath::StartingAt(row), Room::Instance::kYFieldNumber));
    QVariant xScale = _sortedInstances->DataOrDefault(
        FieldPath::Of<Room::Instance>(FieldPath::StartingAt(row), Room::Instance::kXscaleFieldNumber), 1);
    QVariant yScale = _sortedInstances->DataOrDefault(
        FieldPath::Of<Room::Instance>(FieldPath::StartingAt(row), Room::Instance::kYscaleFieldNumber), 1);
    QVariant rot = _sortedInstances->DataOrDefault(
        FieldPath::Of<Room::Instance>(FieldPath::StartingAt(row), Room::Instance::kRotationFieldNumber), 0);

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
