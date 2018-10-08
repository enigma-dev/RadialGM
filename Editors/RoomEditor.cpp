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

  // filter the room preview scroll area to paint
  // the checkerboard transparency pattern and to
  // track the cursor position for the status bar
  ui->roomPreview->widget()->installEventFilter(this);
  // track the cursor position on the transparency pattern for the status bar
  ui->roomPreview->widget()->setMouseTracking(true);
  // also need to track it on the room preview itself so the event is received
  ui->roomRenderer->setMouseTracking(true);

  cursorPositionLabel = new QLabel();
  this->updateCursorPositionLabel(ui->roomPreview->widget()->cursor().pos());
  assetNameLabel = new QLabel("obj_xxx");

  ui->statusBar->addWidget(cursorPositionLabel);
  ui->statusBar->addWidget(assetNameLabel);
}

RoomEditor::~RoomEditor() { delete ui; }

bool RoomEditor::eventFilter(QObject* obj, QEvent* event) {
  if (obj == ui->roomPreview->widget()) {
    if (event->type() == QEvent::Paint) {
      QPainter painter(ui->roomPreview->widget());
      painter.scale(4, 4);
      painter.fillRect(painter.viewport(), ArtManager::GetTransparenyBrush());
      return false;
    } else if (event->type() == QEvent::MouseMove) {
      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
      QPoint roomPos = mouseEvent->pos() - ui->roomRenderer->pos();
      roomPos /= ui->roomRenderer->GetZoom();
      this->updateCursorPositionLabel(roomPos);
    }
  }

  return QWidget::eventFilter(obj, event);
}

void RoomEditor::updateCursorPositionLabel(const QPoint& pos) {
  this->cursorPositionLabel->setText(tr("X %0, Y %1").arg(pos.x()).arg(pos.y()));
}

void RoomEditor::on_actionZoomIn_triggered() { ui->roomRenderer->SetZoom(ui->roomRenderer->GetZoom() * 2); }

void RoomEditor::on_actionZoomOut_triggered() { ui->roomRenderer->SetZoom(ui->roomRenderer->GetZoom() / 2); }

void RoomEditor::on_actionZoom_triggered() { ui->roomRenderer->SetZoom(1.0); }
