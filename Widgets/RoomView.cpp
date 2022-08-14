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
    _sortedInstances->SetSourceModel(model->GetSubModel<RepeatedMessageModel*>(EGMRoom::kInstancesFieldNumber));
    _sortedInstances->sort(EGMRoom::Instance::kObjectTypeFieldNumber);
    _sortedTiles->SetSourceModel(model->GetSubModel<RepeatedMessageModel*>(EGMRoom::kTilesFieldNumber));
    _sortedTiles->sort(EGMRoom::Tile::kDepthFieldNumber);
  }
  setFixedSize(sizeHint());
  repaint();
}

QSize RoomView::sizeHint() const {
  if (!_model) return QSize(640, 480);
  QVariant roomWidth = _model->DataOrDefault(FieldPath::Of<EGMRoom>(EGMRoom::kWidthFieldNumber), 640),
           roomHeight = _model->DataOrDefault(FieldPath::Of<EGMRoom>(EGMRoom::kHeightFieldNumber), 480);
  return QSize(roomWidth.toUInt(), roomHeight.toUInt());
}

void RoomView::Paint(QPainter& painter) {
  _grid.type = GridType::Standard;

  if (!_model) return;

  QVariant hsnap = _model->Data(FieldPath::Of<EGMRoom>(EGMRoom::kHsnapFieldNumber));
  QVariant vsnap = _model->Data(FieldPath::Of<EGMRoom>(EGMRoom::kVsnapFieldNumber));
  _grid.horSpacing = hsnap.isValid() ? hsnap.toInt() : 16;
  _grid.vertSpacing = vsnap.isValid() ? vsnap.toInt() : 16;

  QColor roomColor = QColor(255, 255, 255, 100);

  if (_model->Data(FieldPath::Of<EGMRoom>(EGMRoom::kShowColorFieldNumber)).toBool())
    roomColor = _model->Data(FieldPath::Of<EGMRoom>(EGMRoom::kColorFieldNumber)).toInt();

  QVariant roomWidth = _model->Data(FieldPath::Of<EGMRoom>(EGMRoom::kWidthFieldNumber)),
           roomHeight = _model->Data(FieldPath::Of<EGMRoom>(EGMRoom::kHeightFieldNumber));
  painter.fillRect(
      QRectF(0, 0, roomWidth.isValid() ? roomWidth.toUInt() : 640, roomWidth.isValid() ? roomHeight.toUInt() : 480),
      QBrush(roomColor));

  // to check whether room (in case of tmx importer) is of hexagonal orientation or not
  QString orientation = _model->Data(FieldPath::Of<EGMRoom>(EGMRoom::kOrientationFieldNumber)).toString();

  paintBackgrounds(painter, false);
  paintTiles(painter, orientation == "hexagonal");
  paintInstances(painter);
  paintBackgrounds(painter, true);
}

void RoomView::paintTiles(QPainter& painter, int isHexMap) {
  for (int row = 0; row < _sortedTiles->rowCount(); row++) {
    QVariant bkgName = _sortedTiles->Data(
        FieldPath::Of<EGMRoom::Tile>(FieldPath::StartingAt(row), EGMRoom::Tile::kBackgroundNameFieldNumber));
    MessageModel* bkg = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, bkgName.toString());
    if (!bkg) continue;
    bkg = bkg->GetSubModel<MessageModel*>(TreeNode::kBackgroundFieldNumber);
    if (!bkg) continue;

    // useful in setting source rect correctly
    bool bkgUseAsTileset = bkg->Data(FieldPath::Of<Background>(Background::kUseAsTilesetFieldNumber)).toBool();
    int bkgTileWidth = bkg->Data(FieldPath::Of<Background>(Background::kTileWidthFieldNumber)).toInt();
    int bkgTileHeight = bkg->Data(FieldPath::Of<Background>(Background::kTileHeightFieldNumber)).toInt();
    int bkgImageWidth = bkg->Data(FieldPath::Of<Background>(Background::kWidthFieldNumber)).toInt();
    int bkgImageHeight = bkg->Data(FieldPath::Of<Background>(Background::kHeightFieldNumber)).toInt();

    int x =
        _sortedTiles->Data(FieldPath::Of<EGMRoom::Tile>(FieldPath::StartingAt(row), EGMRoom::Tile::kXFieldNumber)).toInt();
    int y =
        _sortedTiles->Data(FieldPath::Of<EGMRoom::Tile>(FieldPath::StartingAt(row), EGMRoom::Tile::kYFieldNumber)).toInt();
    int xOff =
        _sortedTiles->Data(FieldPath::Of<EGMRoom::Tile>(FieldPath::StartingAt(row), EGMRoom::Tile::kXoffsetFieldNumber))
            .toInt();
    int yOff =
        _sortedTiles->Data(FieldPath::Of<EGMRoom::Tile>(FieldPath::StartingAt(row), EGMRoom::Tile::kYoffsetFieldNumber))
            .toInt();
    int w = _sortedTiles->Data(FieldPath::Of<EGMRoom::Tile>(FieldPath::StartingAt(row), EGMRoom::Tile::kWidthFieldNumber))
                .toInt();
    int h = _sortedTiles->Data(FieldPath::Of<EGMRoom::Tile>(FieldPath::StartingAt(row), EGMRoom::Tile::kHeightFieldNumber))
                .toInt();
    double rotation =
        _sortedTiles->Data(FieldPath::Of<EGMRoom::Tile>(FieldPath::StartingAt(row), EGMRoom::Tile::kRotationFieldNumber)).toDouble();
    bool hasAlpha = false;
    double alpha = _sortedTiles->
        Data(FieldPath::Of<EGMRoom::Tile>(FieldPath::StartingAt(row), EGMRoom::Tile::kAlphaFieldNumber)).toDouble(&hasAlpha);

    QVariant xScale = _sortedTiles->DataOrDefault(
        FieldPath::Of<EGMRoom::Tile>(FieldPath::StartingAt(row), EGMRoom::Tile::kXscaleFieldNumber));
    QVariant yScale = _sortedTiles->DataOrDefault(
        FieldPath::Of<EGMRoom::Tile>(FieldPath::StartingAt(row), EGMRoom::Tile::kYscaleFieldNumber));

    QString imgFile = bkg->Data(FieldPath::Of<Background>(Background::kImageFieldNumber)).toString();
    QPixmap pixmap = ArtManager::GetCachedPixmap(imgFile);
    if (pixmap.isNull()) continue;

    // dest rect handles proper scaling of tile in different scenarios, such as image based background is scaled or
    // case when a given tile of tileset based background is scaled(possible in tiled using objects)
    QRectF dest(x, y, w, h);

    // if background contains multiple tiles then set the rect using tilewidth and tileheight
    // otherwise set it as image width and height
    int bkgUseAsTilesetInt = bkgUseAsTileset; // convert to int to avoid if else branch
    QRectF src = QRectF(xOff, yOff,
                   bkgUseAsTilesetInt*bkgTileWidth + (1-bkgUseAsTilesetInt)*bkgImageWidth,
                   bkgUseAsTilesetInt*bkgTileHeight + (1-bkgUseAsTilesetInt)*bkgImageHeight);

    const QTransform transform = painter.transform();

    // Note: Current rotation support is only according to the location of tiles in ortho and hex Tiled maps,
    // if hexMap is 1(true) tile is rotated from its origin, if hexMap is 0(false) tile is rotate from top-left corner
    // Side node: This translate back-and-forth is to achieve correct transformation in global space
    painter.translate(isHexMap * (x+(w/2))  + (1 - isHexMap) * x,
                      isHexMap * (y+(h/2))  + (1 - isHexMap) * (y+h));
    painter.rotate(rotation);
    painter.translate(isHexMap * (-x-(w/2)) + (1 - isHexMap) * -x,
                      isHexMap * (-y-(h/2)) + (1 - isHexMap) * (-y-h));

    // for scale to work properly, origin must to adjusted to center of pixmap, and its resetted after applying scale
    // Note: scale also handles horizontal and vertical flip of tiles
    painter.translate(x+(w/2),y+(h/2));
    painter.scale(xScale.toFloat(), yScale.toFloat());
    painter.translate(-x-(w/2),-y-(h/2));

    // set opacity
    int hasAlphaInt = hasAlpha; // convert to int to avoid if else branch
    double finalAlpha = hasAlphaInt*alpha + (1-hasAlphaInt)*1.0;
    painter.setOpacity(finalAlpha);

    painter.drawPixmap(dest, pixmap, src);
    painter.setTransform(transform);
  }
}

void RoomView::paintBackgrounds(QPainter& painter, bool foregrounds) {
  RepeatedMessageModel* backgrounds = _model->GetSubModel<RepeatedMessageModel*>(EGMRoom::kBackgroundsFieldNumber);
  for (int row = 0; row < backgrounds->rowCount(); row++) {
    bool visible =
        backgrounds
            ->Data(FieldPath::Of<EGMRoom::Background>(FieldPath::StartingAt(row), EGMRoom::Background::kVisibleFieldNumber))
            .toBool();
    bool foreground = backgrounds
                          ->Data(FieldPath::Of<EGMRoom::Background>(FieldPath::StartingAt(row),
                                                                 EGMRoom::Background::kForegroundFieldNumber))
                          .toBool();
    QString bkgName = backgrounds
                          ->Data(FieldPath::Of<EGMRoom::Background>(FieldPath::StartingAt(row),
                                                                 EGMRoom::Background::kBackgroundNameFieldNumber))
                          .toString();

    if (!visible || foreground != foregrounds) continue;
    MessageModel* bkgRes = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, bkgName);
    if (!bkgRes) continue;
    bkgRes = bkgRes->GetSubModel<MessageModel*>(TreeNode::kBackgroundFieldNumber);
    if (!bkgRes) continue;

    int x =
        backgrounds->Data(FieldPath::Of<EGMRoom::Background>(FieldPath::StartingAt(row), EGMRoom::Background::kXFieldNumber))
            .toInt();
    int y =
        backgrounds->Data(FieldPath::Of<EGMRoom::Background>(FieldPath::StartingAt(row), EGMRoom::Background::kYFieldNumber))
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
            ->Data(FieldPath::Of<EGMRoom::Background>(FieldPath::StartingAt(row), EGMRoom::Background::kStretchFieldNumber))
            .toBool();
    int room_w = _model->Data(FieldPath::Of<EGMRoom>(EGMRoom::kWidthFieldNumber)).toInt();
    int room_h = _model->Data(FieldPath::Of<EGMRoom>(EGMRoom::kHeightFieldNumber)).toInt();

    const QTransform transform = painter.transform();
    if (stretch) {
      painter.scale(room_w / qreal(w), room_h / qreal(h));
    }

    bool hTiled =
        backgrounds
            ->Data(FieldPath::Of<EGMRoom::Background>(FieldPath::StartingAt(row), EGMRoom::Background::kHtiledFieldNumber))
            .toBool();
    bool vTiled =
        backgrounds
            ->Data(FieldPath::Of<EGMRoom::Background>(FieldPath::StartingAt(row), EGMRoom::Background::kVtiledFieldNumber))
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
        FieldPath::Of<EGMRoom::Instance>(FieldPath::StartingAt(row), EGMRoom::Instance::kObjectTypeFieldNumber));

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
        FieldPath::Of<EGMRoom::Instance>(FieldPath::StartingAt(row), EGMRoom::Instance::kXFieldNumber));
    QVariant y = _sortedInstances->Data(
        FieldPath::Of<EGMRoom::Instance>(FieldPath::StartingAt(row), EGMRoom::Instance::kYFieldNumber));
    QVariant xScale = _sortedInstances->DataOrDefault(
        FieldPath::Of<EGMRoom::Instance>(FieldPath::StartingAt(row), EGMRoom::Instance::kXscaleFieldNumber), 1);
    QVariant yScale = _sortedInstances->DataOrDefault(
        FieldPath::Of<EGMRoom::Instance>(FieldPath::StartingAt(row), EGMRoom::Instance::kYscaleFieldNumber), 1);
    QVariant rot = _sortedInstances->DataOrDefault(
        FieldPath::Of<EGMRoom::Instance>(FieldPath::StartingAt(row), EGMRoom::Instance::kRotationFieldNumber), 0);

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
