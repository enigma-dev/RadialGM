#include "RoomRenderer.h"
#include "Components/ArtManager.h"
#include "MainWindow.h"

#include <QPainter>

RoomRenderer::RoomRenderer(QWidget* parent) : QWidget(parent), model(nullptr), zoom(1) {}

void RoomRenderer::SetResourceModel(ProtoModel* model) { this->model = model; }

QSize RoomRenderer::sizeHint() const {
  if (!model) return QSize();
  unsigned roomWidth = model->data(Room::kWidthFieldNumber).toUInt(),
           roomHeight = model->data(Room::kHeightFieldNumber).toUInt();
  return QSize(roomWidth, roomHeight);
}

void RoomRenderer::SetZoom(qreal zoom) {
  if (zoom > 3200) zoom = 3200;
  if (zoom < 0.0625) zoom = 0.0625;
  this->zoom = zoom;
  unsigned roomWidth = model->data(Room::kWidthFieldNumber).toUInt(),
           roomHeight = model->data(Room::kHeightFieldNumber).toUInt();
  setFixedSize(static_cast<unsigned>(roomWidth * zoom) + 1, static_cast<unsigned>(roomHeight * zoom) + 1);
}

const qreal& RoomRenderer::GetZoom() const { return zoom; }

void RoomRenderer::paintEvent(QPaintEvent* /* event */) {
  if (!model) return;
  QPainter painter(this);
  unsigned roomWidth = model->data(Room::kWidthFieldNumber).toUInt(),
           roomHeight = model->data(Room::kHeightFieldNumber).toUInt();

  painter.scale(zoom, zoom);

  Room* room = static_cast<Room*>(model->GetBuffer());
  QColor roomColor = Qt::transparent;
  if (room->show_color()) roomColor = room->color();
  painter.fillRect(QRectF(0, 0, room->width() * zoom, room->height() * zoom), QBrush(roomColor));

  for (auto bkg : room->backgrounds()) {  //TODO: need to draw last if foreground
    if (bkg.visible()) {
      ProtoModel* bkgRes = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, bkg.background_name())
                               ->GetSubModel(TreeNode::kBackgroundFieldNumber);
      if (!bkgRes) continue;
      QString imgFile = bkgRes->data(Background::kImageFieldNumber).toString();
      int w = bkgRes->data(Background::kWidthFieldNumber).toInt();
      int h = bkgRes->data(Background::kHeightFieldNumber).toInt();
      /*
      auto item = scene->addPixmap(ArtManager::GetIcon(imgFile).pixmap(w, h));
      item->setPos(bkg.x(), bkg.y());
      if (bkg.stretch()) {
        item->setTransform(item->transform().scale(room->width() / qreal(w), room->height() / qreal(h)));
      } else {
        if (bkg.htiled() && bkg.vtiled()) {
          for (int i = 0; i < static_cast<int>(room->width()); i += w) {
            for (int j = 0; j < static_cast<int>(room->height()); j += h) {
              auto b = scene->addPixmap(ArtManager::GetIcon(imgFile).pixmap(w, h));
              b->setPos(i, j);
            }
          }
        } else {
          if (bkg.htiled()) {
            for (int i = w; i < static_cast<int>(room->width()); i += w) {
              auto b = scene->addPixmap(ArtManager::GetIcon(imgFile).pixmap(w, h));
              b->setPos(i, 0);
            }
          }
          if (bkg.vtiled()) {
            for (int i = h; i < static_cast<int>(room->height()); i += h) {
              auto b = scene->addPixmap(ArtManager::GetIcon(imgFile).pixmap(w, h));
              b->setPos(0, i);
            }
          }
        }
      }*/
    }
  }

  google::protobuf::RepeatedField<Room::Tile> sortedTiles(room->mutable_tiles()->begin(), room->mutable_tiles()->end());

  std::sort(sortedTiles.begin(), sortedTiles.end(),
            [](const Room::Tile& a, const Room::Tile& b) { return a.depth() > b.depth(); });
  for (auto tile : sortedTiles) {
    ProtoModel* bkg = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, tile.background_name())
                          ->GetSubModel(TreeNode::kBackgroundFieldNumber);
    if (!bkg) continue;
    QString imgFile = bkg->data(Background::kImageFieldNumber).toString();
    int w = static_cast<int>(tile.width());
    int h = static_cast<int>(tile.height());
    /*
    auto item = scene->addPixmap(ArtManager::GetIcon(imgFile).pixmap(w, h));
    item->setPos(tile.x(), tile.y());
    item->setOffset(-tile.xoffset(), -tile.yoffset());
    qreal xscale = (tile.has_xscale()) ? tile.xscale() : 1;
    qreal yscale = (tile.has_yscale()) ? tile.yscale() : 1;
    item->setTransform(item->transform().scale(xscale, yscale));*/
  }

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
    QString imgFile;
    int w = 18;
    int h = 18;
    int xoff = 0;
    int yoff = 0;

    ProtoModel* obj = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, inst.object_type());
    if (!obj) continue;
    obj = obj->GetSubModel(TreeNode::kObjectFieldNumber);
    if (!obj) continue;
    ProtoModel* spr = MainWindow::resourceMap->GetResourceByName(TreeNode::kSprite,
                                                                 obj->data(Object::kSpriteNameFieldNumber).toString());
    if (!spr) continue;
    spr = spr->GetSubModel(TreeNode::kSpriteFieldNumber);
    if (!spr) continue;
    imgFile = spr->GetString(Sprite::kSubimagesFieldNumber, 0);
    w = spr->data(Sprite::kWidthFieldNumber).toInt();
    h = spr->data(Sprite::kHeightFieldNumber).toInt();
    xoff = spr->data(Sprite::kOriginXFieldNumber).toInt();
    yoff = spr->data(Sprite::kOriginYFieldNumber).toInt();

    if (imgFile.isEmpty()) imgFile = "object";

    /*
    auto item = scene->addPixmap();
    item->setPos(inst.x(), inst.y());
    item->setOffset(-xoff, -yoff);
    item->setRotation(inst.rotation());
    item->setTransform(item->transform().scale(inst.xscale(), inst.yscale()));*/
    QPixmap pixmap = ArtManager::GetIcon(imgFile).pixmap(w, h);
    QRect dest(inst.x() - xoff, inst.y() - yoff, w * inst.xscale(), h * inst.yscale());
    painter.drawPixmap(dest, pixmap);
  }

  bool gridVisible = true;
  int gridHorSpacing = 0;
  int gridVertSpacing = 0;
  int gridHorOff = 0;
  int gridVertOff = 0;
  int gridWidth = 16;
  int gridHeight = 16;

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
