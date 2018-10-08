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

  ProtoModel* roomModel = model->GetSubModel(TreeNode::kRoomFieldNumber);
  ui->roomRenderer->SetResourceModel(roomModel);

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

  RepeatedProtoModel* m = roomModel->GetRepeatedSubModel(Room::kInstancesFieldNumber);
  ui->layersAssetsView->setModel(m);
  auto currentInstanceModel = roomModel->data(Room::kInstancesFieldNumber, 0).value<void*>();
  ui->layersPropertiesView->setModel(static_cast<ProtoModel*>(currentInstanceModel));

  for (int c = 0; c < m->columnCount(); ++c) {
    if (c != Room::Instance::kNameFieldNumber && c != Room::Instance::kObjectTypeFieldNumber &&
        c != Room::Instance::kIdFieldNumber)
      ui->layersAssetsView->hideColumn(c);
    else
      ui->layersAssetsView->resizeColumnToContents(c);
  }

  ui->layersAssetsView->header()->swapSections(Room::Instance::kNameFieldNumber,
                                               Room::Instance::kObjectTypeFieldNumber);

  ui->roomPreview->viewport()->installEventFilter(this);
  //ui->roomPreview->setBackgroundRole(QPalette::Dark);
}

RoomEditor::~RoomEditor() { delete ui; }
#include <QDebug>
bool RoomEditor::eventFilter(QObject* obj, QEvent* event) {
  if (obj == ui->roomPreview->viewport()) {
    //TODO: fix the transparency pattern on viewport
    qDebug() << "bing bing bing" << event->type();
    if (event->type() == QEvent::Paint) {
      QPainter painter(ui->roomPreview->viewport());
      painter.scale(4, 4);
      painter.fillRect(painter.viewport(), ArtManager::GetTransparenyBrush());
      return false;
    }
  }

  return QWidget::eventFilter(obj, event);
}

void RoomEditor::on_actionZoomIn_triggered() { ui->roomRenderer->SetZoom(zoom * 2); }

void RoomEditor::on_actionZoomOut_triggered() { ui->roomRenderer->SetZoom(zoom / 2); }

void RoomEditor::on_actionZoom_triggered() { ui->roomRenderer->SetZoom(1.0); }
