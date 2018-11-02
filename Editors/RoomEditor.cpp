#include "RoomEditor.h"
#include "Components/ArtManager.h"
#include "Components/QMenuView.h"
#include "MainWindow.h"
#include "ui_RoomEditor.h"

#include "Models/ImmediateMapper.h"
#include "Models/TreeSortFilterProxyModel.h"
#include "codegen/Room.pb.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMouseEvent>

#include <algorithm>

using View = buffers::resources::Room::View;

RoomEditor::RoomEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::RoomEditor) {
  ui->setupUi(this);

  ProtoModel* roomModel = model->GetSubModel(TreeNode::kRoomFieldNumber);
  ui->roomView->SetResourceModel(roomModel);

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

  RepeatedProtoModel* vm = roomModel->GetRepeatedSubModel(Room::kViewsFieldNumber);
  ImmediateDataWidgetMapper* viewMapper = new ImmediateDataWidgetMapper(this);
  viewMapper->setModel(vm);
  viewMapper->addMapping(ui->viewVisibleCheckBox, View::kVisibleFieldNumber);

  viewMapper->addMapping(ui->cameraXSpinBox, View::kXviewFieldNumber);
  viewMapper->addMapping(ui->cameraYSpinBox, View::kYviewFieldNumber);
  viewMapper->addMapping(ui->cameraWidthSpinBox, View::kWviewFieldNumber);
  viewMapper->addMapping(ui->cameraHeightSpinBox, View::kHviewFieldNumber);

  viewMapper->addMapping(ui->viewportXSpinBox, View::kXportFieldNumber);
  viewMapper->addMapping(ui->viewportYSpinBox, View::kYportFieldNumber);
  viewMapper->addMapping(ui->viewportWidthSpinBox, View::kWportFieldNumber);
  viewMapper->addMapping(ui->viewportHeightSpinBox, View::kHportFieldNumber);

  viewMapper->addMapping(ui->followingHBorderSpinBox, View::kHborderFieldNumber);
  viewMapper->addMapping(ui->followingVBorderSpinBox, View::kVborderFieldNumber);
  viewMapper->addMapping(ui->followingHSpeedSpinBox, View::kHspeedFieldNumber);
  viewMapper->addMapping(ui->followingVSpeedSpinBox, View::kVspeedFieldNumber);
  viewMapper->toFirst();

  QMenuView* objMenu = new QMenuView(this);
  TreeSortFilterProxyModel* treeProxy = new TreeSortFilterProxyModel(this);
  treeProxy->SetFilterType(TreeNode::TypeCase::kObject);
  treeProxy->setSourceModel(MainWindow::treeModel.get());
  objMenu->setModel(treeProxy);
  ui->objectSelectButton->setMenu(objMenu);

  connect(objMenu, &QMenu::triggered, this, &RoomEditor::SelectedObjectChanged);

  connect(ui->currentViewComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          [=](int index) { viewMapper->setCurrentIndex(index); });

  RepeatedProtoModel* m = roomModel->GetRepeatedSubModel(Room::kInstancesFieldNumber);
  ui->layersAssetsView->setModel(m);
  connect(ui->layersAssetsView->selectionModel(), &QItemSelectionModel::selectionChanged,
          [=](const QItemSelection& selected, const QItemSelection& /*deselected*/) {
            if (selected.empty()) return;
            auto selectedIndex = selected.indexes().first();
            auto currentInstanceModel =
                roomModel->data(Room::kInstancesFieldNumber, selectedIndex.row()).value<void*>();
            ui->layersPropertiesView->setModel(static_cast<ProtoModel*>(currentInstanceModel));
          });

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
  ui->roomView->setMouseTracking(true);

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
      QPoint roomPos = mouseEvent->pos() - ui->roomView->pos();
      roomPos /= ui->roomView->GetZoom();
      this->updateCursorPositionLabel(roomPos);
    }
  }

  return QWidget::eventFilter(obj, event);
}

void RoomEditor::SelectedObjectChanged(QAction* action) { ui->currentObject->setText(action->text()); }

void RoomEditor::updateCursorPositionLabel(const QPoint& pos) {
  this->cursorPositionLabel->setText(tr("X %0, Y %1").arg(pos.x()).arg(pos.y()));
}

void RoomEditor::on_actionZoomIn_triggered() { ui->roomView->SetZoom(ui->roomView->GetZoom() * 2); }

void RoomEditor::on_actionZoomOut_triggered() { ui->roomView->SetZoom(ui->roomView->GetZoom() / 2); }

void RoomEditor::on_actionZoom_triggered() { ui->roomView->SetZoom(1.0); }
