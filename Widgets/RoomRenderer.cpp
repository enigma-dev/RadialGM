#include "RoomRenderer.h"
#include "Components/ArtManager.h"
#include "MainWindow.h"

#include <QPainter>

RoomRenderer::RoomRenderer(QWidget* parent) : QWidget(parent), model(nullptr) { this->SetZoom(1.0); }

void RoomRenderer::SetResourceModel(ProtoModel* model) {
  this->model = model;
  this->SetZoom(1.0);
}

QSize RoomRenderer::sizeHint() const {
  if (!model) return QSize(640, 480);
  unsigned roomWidth = model->data(Room::kWidthFieldNumber).toUInt(),
           roomHeight = model->data(Room::kHeightFieldNumber).toUInt();
  return QSize(roomWidth, roomHeight);
}

void RoomRenderer::SetZoom(qreal zoom) {
  if (zoom > 3200) zoom = 3200;
  if (zoom < 0.0625) zoom = 0.0625;
  this->zoom = zoom;
  QSize size(640, 480);
  if (model) {
    size.setWidth(model->data(Room::kWidthFieldNumber).toUInt());
    size.setHeight(model->data(Room::kHeightFieldNumber).toUInt());
  }
  size *= zoom;

  setFixedSize(size);
}

const qreal& RoomRenderer::GetZoom() const { return zoom; }

void RoomRenderer::paintEvent(QPaintEvent* /* event */) {
  if (!model) return;
  QPainter painter(this);

  painter.scale(zoom, zoom);

  Room* room = static_cast<Room*>(model->GetBuffer());
  QColor roomColor = Qt::transparent;
  if (room->show_color()) roomColor = room->color();
  painter.fillRect(QRectF(0, 0, room->width(), room->height()), QBrush(roomColor));

  this->paintBackgrounds(painter, room, false);
  this->paintTiles(painter, room);
  this->paintInstances(painter, room);
  this->paintBackgrounds(painter, room, true);
  this->paintGrid(painter, room);
}

void RoomRenderer::paintTiles(QPainter& painter, Room* room) {
  google::protobuf::RepeatedField<Room::Tile> sortedTiles(room->mutable_tiles()->begin(), room->mutable_tiles()->end());

  std::sort(sortedTiles.begin(), sortedTiles.end(),
            [](const Room::Tile& a, const Room::Tile& b) { return a.depth() > b.depth(); });
  for (auto tile : sortedTiles) {
    ProtoModel* bkg = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, tile.background_name());
    if (!bkg) continue;
    bkg = bkg->GetSubModel(TreeNode::kBackgroundFieldNumber);
    if (!bkg) continue;
    int w = static_cast<int>(tile.width());
    int h = static_cast<int>(tile.height());

    QString imgFile = bkg->data(Background::kImageFieldNumber).toString();
    QPixmap pixmap = ArtManager::GetCachedPixmap(imgFile);
    if (pixmap.isNull()) continue;

    QRect dest(tile.x(), tile.y(), w, h);
    QRect src(tile.xoffset(), tile.yoffset(), w, h);
    const QTransform transform = painter.transform();
    painter.scale(tile.has_xscale() ? tile.xscale() : 1, tile.has_yscale() ? tile.yscale() : 1);
    painter.drawPixmap(dest, pixmap, src);
    painter.setTransform(transform);
  }
}

void RoomRenderer::paintBackgrounds(QPainter& painter, Room* room, bool foregrounds) {
  for (auto bkg : room->backgrounds()) {  //TODO: need to draw last if foreground
    if (!bkg.visible() || bkg.foreground() != foregrounds) continue;
    ProtoModel* bkgRes = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, bkg.background_name());
    if (!bkgRes) continue;
    bkgRes = bkgRes->GetSubModel(TreeNode::kBackgroundFieldNumber);
    if (!bkgRes) continue;
    int w = bkgRes->data(Background::kWidthFieldNumber).toInt();
    int h = bkgRes->data(Background::kHeightFieldNumber).toInt();

    QString imgFile = bkgRes->data(Background::kImageFieldNumber).toString();
    QPixmap pixmap = ArtManager::GetCachedPixmap(imgFile);
    if (pixmap.isNull()) continue;

    QRect dest(bkg.x(), bkg.y(), w, h);
    QRect src(0, 0, w, h);
    const QTransform transform = painter.transform();
    if (bkg.stretch()) {
      painter.scale(room->width() / qreal(w), room->height() / qreal(h));
    }
    if (bkg.htiled()) {
      dest.setX(0);
      dest.setWidth(room->width());
      src.setX(bkg.x());
    }
    if (bkg.vtiled()) {
      dest.setY(0);
      dest.setHeight(room->height());
      src.setY(bkg.y());
    }
    painter.fillRect(dest, QBrush(pixmap));
    painter.setTransform(transform);
  }
}

void RoomRenderer::paintInstances(QPainter& painter, Room* room) {
  google::protobuf::RepeatedField<Room::Instance> sortedInstances(room->mutable_instances()->begin(),
                                                                  room->mutable_instances()->end());

  std::sort(sortedInstances.begin(), sortedInstances.end(), [](const Room::Instance& a, const Room::Instance& b) {
    ProtoModel* objA = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, a.object_type())
                           ->GetSubModel(TreeNode::kObjectFieldNumber);
    ProtoModel* objB = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, b.object_type())
                           ->GetSubModel(TreeNode::kObjectFieldNumber);
    if (objA != nullptr && objB != nullptr)
      return objA->data(Object::kDepthFieldNumber) > objB->data(Object::kDepthFieldNumber);
    return false;
  });
  for (auto inst : sortedInstances) {
    QString imgFile = ":/actions/help.png";
    int w = 16;
    int h = 16;
    int xoff = 0;
    int yoff = 0;

    ProtoModel* obj = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, inst.object_type());
    if (!obj) continue;
    obj = obj->GetSubModel(TreeNode::kObjectFieldNumber);
    if (!obj) continue;
    const QString spriteName = obj->data(Object::kSpriteNameFieldNumber).toString();
    ProtoModel* spr = MainWindow::resourceMap->GetResourceByName(TreeNode::kSprite, spriteName);
    if (spr) {
      spr = spr->GetSubModel(TreeNode::kSpriteFieldNumber);
      if (spr) {
        imgFile = spr->GetString(Sprite::kSubimagesFieldNumber, 0);
        w = spr->data(Sprite::kWidthFieldNumber).toInt();
        h = spr->data(Sprite::kHeightFieldNumber).toInt();
        xoff = spr->data(Sprite::kOriginXFieldNumber).toInt();
        yoff = spr->data(Sprite::kOriginYFieldNumber).toInt();
      }
    }

    if (imgFile.isEmpty()) imgFile = "object";
    QPixmap pixmap = ArtManager::GetCachedPixmap(imgFile);
    if (pixmap.isNull()) continue;

    QRect dest(inst.x(), inst.y(), w, h);
    const QTransform transform = painter.transform();
    painter.translate(-xoff, -yoff);
    painter.rotate(inst.rotation());
    painter.scale(inst.xscale(), inst.yscale());
    painter.drawPixmap(dest, pixmap);
    painter.setTransform(transform);
  }
}

void RoomRenderer::paintGrid(QPainter& painter, Room* room) {
  bool gridVisible = true;
  int gridHorSpacing = 0;
  int gridVertSpacing = 0;
  int gridHorOff = 0;
  int gridVertOff = 0;
  int gridWidth = 16;
  int gridHeight = 16;

  unsigned roomWidth = room->width(), roomHeight = room->height();

  if (gridVisible) {
    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    painter.setPen(QColor(0xff, 0xff, 0xff));

    if (gridHorSpacing != 0 || gridVertSpacing != 0) {
      for (int x = gridHorOff; x < roomWidth; x += gridWidth + gridHorSpacing) {
        for (int y = gridVertOff; y < roomHeight; y += gridHeight + gridVertSpacing) {
          painter.drawRect(x, y, gridWidth, gridHeight);
        }
      }
    }

    if (gridHorSpacing == 0) {
      for (int x = gridHorOff; x <= roomWidth; x += gridWidth) painter.drawLine(x, 0, x, roomHeight);
    }

    if (gridVertSpacing == 0) {
      for (int y = gridVertOff; y <= roomWidth; y += gridHeight) painter.drawLine(0, y, roomWidth, y);
    }
  }
}
