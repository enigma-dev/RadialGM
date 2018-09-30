#include "RoomEditor.h"
#include "Components/ArtManager.h"
#include "MainWindow.h"
#include "ui_RoomEditor.h"

#include "codegen/Room.pb.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

using namespace buffers::resources;

RoomEditor::RoomEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::RoomEditor) {
  ui->setupUi(this);

  mapper->addMapping(ui->speedSpinBox, Room::kSpeedFieldNumber);
  mapper->addMapping(ui->widthSpinBox, Room::kWidthFieldNumber);
  mapper->addMapping(ui->heightSpinBox, Room::kHeightFieldNumber);
  mapper->addMapping(ui->clearCheckBox, Room::kClearDisplayBufferFieldNumber);
  mapper->addMapping(ui->persistentCheckBox, Room::kPersistentFieldNumber);
  mapper->addMapping(ui->captionLineEdit, Room::kCaptionFieldNumber);

  mapper->addMapping(ui->enableViewsCheckBox, Room::kEnableViewsFieldNumber);
  mapper->addMapping(ui->clearViewportCheckBox, Room::kClearViewBackgroundFieldNumber);
  mapper->toFirst();

  Room* room = static_cast<Room*>(model->GetBuffer());

  QGraphicsScene* scene = new QGraphicsScene(0, 0, room->width(), room->height(), this);
  scene->addRect(scene->sceneRect(), QPen(Qt::gray), QBrush(Qt::gray));

  for (auto inst : room->instances()) {
    auto obj = MainWindow::resourceMap->GetResourceByName(buffers::TreeNode::kObject, inst.object_type());
    auto spr = MainWindow::resourceMap->GetResourceByName(buffers::TreeNode::kSprite, obj->data(buffers::resources::Object::kSpriteNameFieldNumber).toString());
    QString imgFile = spr->GetString(buffers::resources::Sprite::kSubimagesFieldNumber, 0);
    auto item = scene->addPixmap(QPixmap(imgFile));
    item->setPos(inst.x(), inst.y());
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
