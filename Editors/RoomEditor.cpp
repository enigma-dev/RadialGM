#include "RoomEditor.h"
#include "Components/ArtManager.h"
#include "MainWindow.h"
#include "ui_RoomEditor.h"

#include "codegen/Room.pb.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

#include <algorithm>

RoomEditor::RoomEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::RoomEditor) {
  ui->setupUi(this);

  nodeMapper->addMapping(ui->roomName, TreeNode::kNameFieldNumber);
  nodeMapper->toFirst();

  resMapper->addMapping(ui->speedSpinBox, Room::kSpeedFieldNumber);
  resMapper->addMapping(ui->widthSpinBox, Room::kWidthFieldNumber);
  resMapper->addMapping(ui->heightSpinBox, Room::kHeightFieldNumber);
  resMapper->addMapping(ui->clearCheckBox, Room::kClearDisplayBufferFieldNumber);
  resMapper->addMapping(ui->persistentCheckBox, Room::kPersistentFieldNumber);
  resMapper->addMapping(ui->captionLineEdit, Room::kCaptionFieldNumber);

  resMapper->addMapping(ui->enableViewsCheckBox, Room::kEnableViewsFieldNumber);
  resMapper->addMapping(ui->clearViewportCheckBox, Room::kClearViewBackgroundFieldNumber);
  resMapper->toFirst();

  ProtoModel* roomModel = model->GetSubModel(TreeNode::kRoomFieldNumber);

  Room* room = static_cast<Room*>(roomModel->GetBuffer());

  QGraphicsScene* scene = new QGraphicsScene(0, 0, room->width(), room->height(), this);

  QColor roomColor = Qt::transparent;
  if (room->show_color()) roomColor = room->color();
  scene->addRect(scene->sceneRect(), QPen(roomColor), QBrush(roomColor));

  for (auto bkg : room->backgrounds()) { //TODO: need to draw last if foreground
      if (bkg.visible()) {
        ProtoModel* bkgRes = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground,
                               bkg.background_name())->GetSubModel(TreeNode::kBackgroundFieldNumber);
        if (bkgRes != nullptr) {
          QString imgFile = bkgRes->data(Background::kImageFieldNumber).toString();
          int w = bkgRes->data(Background::kWidthFieldNumber).toInt();
          int h = bkgRes->data(Background::kHeightFieldNumber).toInt();
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
        }
      }
    }
  }


  google::protobuf::RepeatedField<Room::Tile>
          sortedTiles(room->mutable_tiles()->begin(), room->mutable_tiles()->end());

  std::sort(sortedTiles.begin(), sortedTiles.end(),
            [](const Room::Tile& a, const Room::Tile& b) {
               return a.depth() > b.depth();
            });
  for (auto tile : sortedTiles) {

    ProtoModel* bkg = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground,
                        tile.background_name())->GetSubModel(TreeNode::kBackgroundFieldNumber);
    if (bkg != nullptr) {
      QString imgFile = bkg->data(Background::kImageFieldNumber).toString();
      int w = static_cast<int>(tile.width());
      int h = static_cast<int>(tile.height());
      auto item = scene->addPixmap(ArtManager::GetIcon(imgFile).pixmap(w, h));
      item->setPos(tile.x(), tile.y());
      item->setOffset(-tile.xoffset(), -tile.yoffset());
      qreal xscale = (tile.has_xscale()) ? tile.xscale() : 1;
      qreal yscale = (tile.has_yscale()) ? tile.yscale() : 1;
      item->setTransform(item->transform().scale(xscale, yscale));
    }
  }

  QList<QVariant> instances = resMapper->data(Room::kInstancesFieldNumber).toList();
  ui->layersTableView->setModel(static_cast<ProtoModel*>(instances[0].value<void*>()));

  google::protobuf::RepeatedField<Room::Instance>
          sortedInstances(room->mutable_instances()->begin(), room->mutable_instances()->end());

  std::sort(sortedInstances.begin(), sortedInstances.end(),
            [](const Room::Instance& a, const Room::Instance& b) {
               ProtoModel* objA = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, a.object_type())->GetSubModel(TreeNode::kObjectFieldNumber);
               ProtoModel* objB = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, b.object_type())->GetSubModel(TreeNode::kObjectFieldNumber);
               if (objA != nullptr && objB != nullptr)
                 return objA->data(Object::kDepthFieldNumber) >
                         objB->data(Object::kDepthFieldNumber);
               return false;
            });
  for (auto inst : sortedInstances) {
    QString imgFile;
    int w = 18;
    int h = 18;
    int xoff = 0;
    int yoff = 0;

    ProtoModel* obj = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, inst.object_type());
    if (obj != nullptr) {
      obj = obj->GetSubModel(TreeNode::kObjectFieldNumber);
      ProtoModel* spr = MainWindow::resourceMap->GetResourceByName(TreeNode::kSprite, obj->data(Object::kSpriteNameFieldNumber).toString());
      if (spr != nullptr) {
        spr = spr->GetSubModel(TreeNode::kSpriteFieldNumber);
        imgFile = spr->GetString(Sprite::kSubimagesFieldNumber, 0);
        w = spr->data(Sprite::kWidthFieldNumber).toInt();
        h = spr->data(Sprite::kHeightFieldNumber).toInt();
        xoff = spr->data(Sprite::kOriginXFieldNumber).toInt();
        yoff = spr->data(Sprite::kOriginYFieldNumber).toInt();
      }
    }

    if (imgFile.isEmpty())
      imgFile = "object";

    auto item = scene->addPixmap(ArtManager::GetIcon(imgFile).pixmap(w, h));
    item->setPos(inst.x(), inst.y());
    item->setOffset(-xoff, -yoff);
    item->setRotation(inst.rotation());
    item->setTransform(item->transform().scale(inst.xscale(), inst.yscale()));
  }

  ui->graphicsView->viewport()->installEventFilter(this);
  ui->graphicsView->setScene(scene);
}

RoomEditor::~RoomEditor() { delete ui; }

bool RoomEditor::eventFilter(QObject* obj, QEvent* event) {
  if (obj == ui->graphicsView->viewport()) {
    if (event->type() == QEvent::Paint) {
      QPainter painter(ui->graphicsView->viewport());
      painter.scale(4, 4);
      painter.fillRect(painter.viewport(), ArtManager::GetTransparenyBrush());
      return false;
    }
  }

  return QWidget::eventFilter(obj, event);
}

void RoomEditor::setZoom(qreal zoom) {
  if (zoom > 3200) zoom = 3200;
  if (zoom < 0.0625) zoom = 0.0625;
  this->zoom = zoom;
  ui->graphicsView->setTransform(QTransform::fromScale(zoom, zoom));
}

void RoomEditor::on_actionZoomIn_triggered() { this->setZoom(zoom * 2); }

void RoomEditor::on_actionZoomOut_triggered() { this->setZoom(zoom / 2); }

void RoomEditor::on_actionZoom_triggered() { this->setZoom(1.0); }
