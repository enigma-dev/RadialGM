#include "PathEditor.h"
#include "Components/ArtManager.h"
#include "Widgets/ResourceSelector.h"

#include "ui_PathEditor.h"

#include <QEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QSpinBox>
#include <QToolButton>

PathEditor::PathEditor(ProtoModelPtr model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::PathEditor) {
  ui->setupUi(this);
  connect(ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);

  ui->pathPreviewBackground->SetAssetView(ui->roomView);

  // Prefer resizing the path view over the points editor
  ui->splitter->setStretchFactor(0, 0);
  ui->splitter->setStretchFactor(1, 1);

  ui->mainToolBar->setStyleSheet("QToolBar{spacing:8px;}");

  QLabel* xSnapLabel(new QLabel(tr("X Snap"), this));
  ui->mainToolBar->addWidget(xSnapLabel);

  QSpinBox* xSnap(new QSpinBox(this));
  ui->mainToolBar->addWidget(xSnap);
  xSnap->setRange(1, 99999999);

  QLabel* ySnapLabel(new QLabel(tr("Y Snap"), this));
  ui->mainToolBar->addWidget(ySnapLabel);

  QSpinBox* ySnap(new QSpinBox(this));
  ui->mainToolBar->addWidget(ySnap);
  ySnap->setRange(1, 99999999);

  ui->mainToolBar->addSeparator();

  ResourceSelector* roomButton(new ResourceSelector(this, TreeNode::TypeCase::kRoom));
  roomButton->setPopupMode(QToolButton::MenuButtonPopup);
  roomButton->setIcon(ArtManager::GetIcon(":/resources/room.png"));
  ui->mainToolBar->addWidget(roomButton);

  roomLineEdit = new QLineEdit(this);
  roomLineEdit->setReadOnly(true);
  roomLineEdit->setMaximumWidth(150);
  ui->mainToolBar->addWidget(roomLineEdit);

  connect(roomButton, &QToolButton::pressed, this, &PathEditor::RoomMenuButtonPressed);
  connect(roomButton->menu, &QMenu::triggered, this, &PathEditor::RoomMenuItemSelected);

  ui->mainToolBar->addSeparator();

  QToolButton* zoom(new QToolButton(this));
  zoom->setIcon(ArtManager::GetIcon(":/actions/zoom.png"));
  ui->mainToolBar->addWidget(zoom);
  connect(zoom, &QToolButton::pressed, ui->pathPreviewBackground, &AssetScrollAreaBackground::ResetZoom);

  QToolButton* zoomIn(new QToolButton(this));
  zoomIn->setIcon(ArtManager::GetIcon(":/actions/zoom-in.png"));
  ui->mainToolBar->addWidget(zoomIn);
  connect(zoomIn, &QToolButton::pressed, ui->pathPreviewBackground, &AssetScrollAreaBackground::ZoomIn);

  QToolButton* zoomOut(new QToolButton(this));
  zoomOut->setIcon(ArtManager::GetIcon(":/actions/zoom-out.png"));
  ui->mainToolBar->addWidget(zoomOut);
  connect(zoomOut, &QToolButton::pressed, ui->pathPreviewBackground, &AssetScrollAreaBackground::ZoomOut);

  connect(ui->actionShowGrid, &QAction::triggered, ui->pathPreviewBackground,
          &AssetScrollAreaBackground::SetGridVisible);

  connect(ui->actionSnaptoGrid, &QAction::triggered, this, &PathEditor::SetSnapToGrid);

  cursorPositionLabel = new QLabel();

  ui->statusBar->addWidget(cursorPositionLabel);

  ui->pointsTableView->installEventFilter(this);

  connect(xSnap, QOverload<int>::of(&QSpinBox::valueChanged), ui->pathPreviewBackground,
          &AssetScrollAreaBackground::SetGridHSnap);
  connect(ySnap, QOverload<int>::of(&QSpinBox::valueChanged), ui->pathPreviewBackground,
          &AssetScrollAreaBackground::SetGridVSnap);

  connect(this, &BaseEditor::FocusGained, [this]() { ui->pathPreviewBackground->SetParentHasFocus(true); });
  connect(this, &BaseEditor::FocusLost, [this]() { ui->pathPreviewBackground->SetParentHasFocus(false); });

  nodeMapper->addMapping(ui->nameEdit, TreeNode::kNameFieldNumber);

  resMapper->addMapping(ui->smoothCheckBox, Path::kSmoothFieldNumber);
  resMapper->addMapping(ui->closedCheckBox, Path::kClosedFieldNumber);
  resMapper->addMapping(ui->precisionSpinBox, Path::kPrecisionFieldNumber);
  resMapper->addMapping(xSnap, Path::kHsnapFieldNumber);
  resMapper->addMapping(ySnap, Path::kVsnapFieldNumber);

  RebindSubModels();
}

PathEditor::~PathEditor() { delete ui; }

void PathEditor::RebindSubModels() {
  pathModel = _model->GetSubModel(TreeNode::kPathFieldNumber);

  ui->roomView->SetPathModel(pathModel);
  pointsModel = pathModel->GetRepeatedSubModel(Path::kPointsFieldNumber);
  ui->pointsTableView->setModel(pointsModel);
  ui->pointsTableView->hideColumn(0);

  QString roomName = pathModel->data(Path::kBackgroundRoomNameFieldNumber).toString();
  if (roomName != "") {
    ui->roomView->SetResourceModel(
        MainWindow::resourceMap->GetResourceByName(TypeCase::kRoom, roomName)->GetSubModel(TreeNode::kRoomFieldNumber));
  }
  roomLineEdit->setText(roomName);

  connect(ui->pathPreviewBackground, &AssetScrollAreaBackground::MouseMoved, this, &PathEditor::MouseMoved);
  connect(ui->pathPreviewBackground, &AssetScrollAreaBackground::MousePressed, this, &PathEditor::MousePressed);
  connect(ui->pathPreviewBackground, &AssetScrollAreaBackground::MouseReleased, this, &PathEditor::MouseReleased);

  connect(ui->pointsTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
          &PathEditor::UpdateSelection);

  ui->deletePointButton->setDisabled(true);

  BaseEditor::RebindSubModels();
}

bool PathEditor::eventFilter(QObject* obj, QEvent* event) {
  if (pointsModel != nullptr) {
    // Resize columns to view size
    if (obj == ui->pointsTableView && event->type() == QEvent::Resize) {
      QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
      int cc = pointsModel->columnCount() - 1;
      for (int c = 1; c < cc; ++c) {  // column 1 is hidden
        ui->pointsTableView->setColumnWidth(c, (resizeEvent->size().width()) / cc);
      }
    }
  }
  return QWidget::eventFilter(obj, event);
}

void PathEditor::InsertPoint(int index, int x, int y, int speed) {
  pointsModel->insertRow(index);
  pointsModel->setData(index, Path::Point::kXFieldNumber, x);
  pointsModel->setData(index, Path::Point::kYFieldNumber, y);
  pointsModel->setData(index, Path::Point::kSpeedFieldNumber, speed);
  ui->roomView->selectedPointIndex = index;
}

void PathEditor::SetSnapToGrid(bool snap) { this->snapToGrid = snap; }

void PathEditor::RoomMenuItemSelected(QAction* action) {
  roomLineEdit->setText(action->text());
  ui->roomView->SetResourceModel(MainWindow::resourceMap->GetResourceByName(TypeCase::kRoom, action->text())
                                     ->GetSubModel(TreeNode::kRoomFieldNumber));
  ui->pathPreviewBackground->SetZoom(1);
  pathModel->setData(Path::kBackgroundRoomNameFieldNumber, 0, action->text());
}

void PathEditor::RoomMenuButtonPressed() {
  // Manually clear selected room because we cant use a mapping here
  roomLineEdit->setText("");
  ui->roomView->SetResourceModel(nullptr);
  pathModel->setData(Path::kBackgroundRoomNameFieldNumber, 0, "");
}

void PathEditor::MouseMoved(int x, int y) {
  const GridDimensions g = ui->roomView->GetGrid();
  QPoint mousePos(x, y);
  if (snapToGrid) mousePos = QPoint(RoundNum(x, g.horSpacing), RoundNum(y, g.vertSpacing));
  ui->roomView->mousePos = mousePos;
  cursorPositionLabel->setText(tr("X %0, Y %1").arg(mousePos.x()).arg(mousePos.y()));
  if (draggingPoint) {
    pointsModel->setData(ui->roomView->selectedPointIndex, Path::Point::kXFieldNumber, ui->roomView->mousePos.x());
    pointsModel->setData(ui->roomView->selectedPointIndex, Path::Point::kYFieldNumber, ui->roomView->mousePos.y());
  } else {
    ui->pathPreviewBackground->update();  // manually call update to redraw cursor if no data changes
  }
}

void PathEditor::MousePressed(Qt::MouseButton button) {
  if (button == Qt::MouseButton::LeftButton) {
    draggingPoint = true;
    // Check for point at location (reverse loop to get last added one if dups)
    for (int i = pointsModel->rowCount() - 1; i >= 0; i--) {
      QPoint pt = ui->roomView->Point(i);
      if (pt == ui->roomView->mousePos) {
        QModelIndex newSelectIndex = pointsModel->index(i, Path::Point::kXFieldNumber);
        ui->pointsTableView->setCurrentIndex(newSelectIndex);
        ui->pointsTableView->selectionModel()->select(newSelectIndex,
                                                      QItemSelectionModel::QItemSelectionModel::ClearAndSelect);
        return;
      }
    }
    // No point found. Add one and select it
    InsertPoint(pointsModel->rowCount(), ui->roomView->mousePos.x(), ui->roomView->mousePos.y(),
                ui->speedSpinBox->value());
  }
}

void PathEditor::MouseReleased(Qt::MouseButton button) {
  if (button == Qt::MouseButton::LeftButton) {
    draggingPoint = false;
  }
}

void PathEditor::UpdateSelection(const QItemSelection& selected, const QItemSelection& /*deselected*/) {
  int selectIndex = -1;
  if (!selected.indexes().empty()) selectIndex = selected.indexes()[0].row();
  ui->roomView->selectedPointIndex = selectIndex;
  ui->pathPreviewBackground->update();
  ui->deletePointButton->setDisabled((selectIndex == -1));
}

void PathEditor::on_addPointButton_pressed() {
  InsertPoint(pointsModel->rowCount(), ui->xSpinBox->value(), ui->yspinBox->value(), ui->speedSpinBox->value());
}

void PathEditor::on_insertPointButton_pressed() {
  int insertIndex = ui->pointsTableView->selectionModel()->currentIndex().row();
  if (insertIndex < 0) insertIndex = 0;
  InsertPoint(insertIndex, ui->xSpinBox->value(), ui->yspinBox->value(), ui->speedSpinBox->value());
}

void PathEditor::on_deletePointButton_pressed() {
  int deleteIndex = ui->pointsTableView->selectionModel()->currentIndex().row();
  {
    RepeatedProtoModel::RowRemovalOperation remover(pointsModel);
    remover.RemoveRow(deleteIndex);
  }

  if (pointsModel->rowCount() > 0) {
    QModelIndex newSelectIndex =
        pointsModel->index((deleteIndex == 0) ? 0 : deleteIndex - 1, Path::Point::kXFieldNumber);
    ui->pointsTableView->setCurrentIndex(newSelectIndex);
    ui->pointsTableView->selectionModel()->select(newSelectIndex,
                                                  QItemSelectionModel::QItemSelectionModel::ClearAndSelect);
  } else {
    ui->deletePointButton->setDisabled(true);
    ui->pointsTableView->selectionModel()->clearSelection();
    ui->pathPreviewBackground->update();
  }
}
