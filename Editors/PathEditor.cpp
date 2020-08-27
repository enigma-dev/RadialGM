#include "PathEditor.h"
#include "Components/ArtManager.h"
#include "Widgets/ResourceSelector.h"

#include "ui_PathEditor.h"

#include <QEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QSpinBox>
#include <QToolButton>

PathEditor::PathEditor(MessageModel* model, QWidget* parent) : BaseEditor(model, parent), _ui(new Ui::PathEditor) {
  _ui->setupUi(this);
  connect(_ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);

  _ui->pathPreviewBackground->SetAssetView(_ui->roomView);

  // Prefer resizing the path view over the points editor
  _ui->splitter->setStretchFactor(0, 0);
  _ui->splitter->setStretchFactor(1, 1);

  _ui->mainToolBar->setStyleSheet("QToolBar{spacing:8px;}");

  QLabel* xSnapLabel(new QLabel(tr("X Snap"), this));
  _ui->mainToolBar->addWidget(xSnapLabel);

  QSpinBox* xSnap(new QSpinBox(this));
  _ui->mainToolBar->addWidget(xSnap);
  xSnap->setRange(1, 99999999);

  QLabel* ySnapLabel(new QLabel(tr("Y Snap"), this));
  _ui->mainToolBar->addWidget(ySnapLabel);

  QSpinBox* ySnap(new QSpinBox(this));
  _ui->mainToolBar->addWidget(ySnap);
  ySnap->setRange(1, 99999999);

  _ui->mainToolBar->addSeparator();

  ResourceSelector* roomButton(new ResourceSelector(this, TreeNode::TypeCase::kRoom));
  roomButton->setPopupMode(QToolButton::MenuButtonPopup);
  roomButton->setIcon(ArtManager::GetIcon(":/resources/room.png"));
  _ui->mainToolBar->addWidget(roomButton);

  _roomLineEdit = new QLineEdit(this);
  _roomLineEdit->setReadOnly(true);
  _roomLineEdit->setMaximumWidth(150);
  _ui->mainToolBar->addWidget(_roomLineEdit);

  connect(roomButton, &QToolButton::pressed, this, &PathEditor::RoomMenuButtonPressed);
  connect(roomButton->_menu, &QMenu::triggered, this, &PathEditor::RoomMenuItemSelected);

  _ui->mainToolBar->addSeparator();

  QToolButton* zoom(new QToolButton(this));
  zoom->setIcon(ArtManager::GetIcon(":/actions/zoom.png"));
  _ui->mainToolBar->addWidget(zoom);
  connect(zoom, &QToolButton::pressed, _ui->pathPreviewBackground, &AssetScrollAreaBackground::ResetZoom);

  QToolButton* zoomIn(new QToolButton(this));
  zoomIn->setIcon(ArtManager::GetIcon(":/actions/zoom-in.png"));
  _ui->mainToolBar->addWidget(zoomIn);
  connect(zoomIn, &QToolButton::pressed, _ui->pathPreviewBackground, &AssetScrollAreaBackground::ZoomIn);

  QToolButton* zoomOut(new QToolButton(this));
  zoomOut->setIcon(ArtManager::GetIcon(":/actions/zoom-out.png"));
  _ui->mainToolBar->addWidget(zoomOut);
  connect(zoomOut, &QToolButton::pressed, _ui->pathPreviewBackground, &AssetScrollAreaBackground::ZoomOut);

  connect(_ui->actionShowGrid, &QAction::triggered, _ui->pathPreviewBackground,
          &AssetScrollAreaBackground::SetGridVisible);

  connect(_ui->actionSnaptoGrid, &QAction::triggered, this, &PathEditor::SetSnapToGrid);

  _cursorPositionLabel = new QLabel();

  _ui->statusBar->addWidget(_cursorPositionLabel);

  // keep the vertical row heights only as big as necessary
  _ui->pointsTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  // evenly divide the width of the list view up for each column
  _ui->pointsTableView->installEventFilter(this);

  connect(xSnap, QOverload<int>::of(&QSpinBox::valueChanged), _ui->pathPreviewBackground,
          &AssetScrollAreaBackground::SetGridHSnap);
  connect(ySnap, QOverload<int>::of(&QSpinBox::valueChanged), _ui->pathPreviewBackground,
          &AssetScrollAreaBackground::SetGridVSnap);

  _nodeMapper->addMapping(_ui->nameEdit, TreeNode::kNameFieldNumber);

  _resMapper->addMapping(_ui->smoothCheckBox, Path::kSmoothFieldNumber);
  _resMapper->addMapping(_ui->closedCheckBox, Path::kClosedFieldNumber);
  _resMapper->addMapping(_ui->precisionSpinBox, Path::kPrecisionFieldNumber);
  _resMapper->addMapping(xSnap, Path::kHsnapFieldNumber);
  _resMapper->addMapping(ySnap, Path::kVsnapFieldNumber);

  RebindSubModels();
}

PathEditor::~PathEditor() { delete _ui; }

void PathEditor::RebindSubModels() {
  _pathModel = _model->GetSubModel<MessageModel*>(TreeNode::kPathFieldNumber);

  _ui->roomView->SetPathModel(_pathModel);
  _pointsModel = _pathModel->GetSubModel<RepeatedMessageModel*>(Path::kPointsFieldNumber);
  _ui->pointsTableView->setModel(_pointsModel);
  _ui->pointsTableView->hideColumn(0);

  QString roomName = _pathModel->Data(Path::kBackgroundRoomNameFieldNumber).toString();
  if (roomName != "") {
    _ui->roomView->SetResourceModel(MainWindow::resourceMap->GetResourceByName(TypeCase::kRoom, roomName)
                                        ->GetSubModel<MessageModel*>(TreeNode::kRoomFieldNumber));
  }
  _roomLineEdit->setText(roomName);

  connect(_ui->pathPreviewBackground, &AssetScrollAreaBackground::MouseMoved, this, &PathEditor::MouseMoved);
  connect(_ui->pathPreviewBackground, &AssetScrollAreaBackground::MousePressed, this, &PathEditor::MousePressed);
  connect(_ui->pathPreviewBackground, &AssetScrollAreaBackground::MouseReleased, this, &PathEditor::MouseReleased);

  connect(_ui->pointsTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
          &PathEditor::UpdateSelection);

  _ui->deletePointButton->setDisabled(true);

  BaseEditor::RebindSubModels();
}

bool PathEditor::eventFilter(QObject* obj, QEvent* event) {
  if (_pointsModel != nullptr) {
    // Resize columns to view size
    if (obj == _ui->pointsTableView && event->type() == QEvent::Resize) {
      QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
      int cc = _pointsModel->columnCount() - 1;
      for (int c = 1; c < cc; ++c) {  // column 1 is hidden
        _ui->pointsTableView->setColumnWidth(c, (resizeEvent->size().width()) / cc);
      }
    }
  }
  return QWidget::eventFilter(obj, event);
}

void PathEditor::InsertPoint(int index, int x, int y, int speed) {
  _pointsModel->insertRow(index);
  _pointsModel->SetData(x, index, Path::Point::kXFieldNumber);
  _pointsModel->SetData(y, index, Path::Point::kYFieldNumber);
  _pointsModel->SetData(speed, index, Path::Point::kSpeedFieldNumber);
  _ui->roomView->selectedPointIndex = index;
}

void PathEditor::SetSnapToGrid(bool snap) { this->_snapToGrid = snap; }

void PathEditor::RoomMenuItemSelected(QAction* action) {
  _roomLineEdit->setText(action->text());
  _ui->roomView->SetResourceModel(MainWindow::resourceMap->GetResourceByName(TypeCase::kRoom, action->text())
                                      ->GetSubModel<MessageModel*>(TreeNode::kRoomFieldNumber));
  _ui->pathPreviewBackground->SetZoom(1);
  _pathModel->SetData(action->text(), Path::kBackgroundRoomNameFieldNumber, 0);
}

void PathEditor::RoomMenuButtonPressed() {
  // Manually clear selected room because we cant use a mapping here
  _roomLineEdit->setText("");
  _ui->roomView->SetResourceModel(nullptr);
  _pathModel->SetData("", Path::kBackgroundRoomNameFieldNumber, 0);
}

void PathEditor::MouseMoved(int x, int y) {
  const GridDimensions g = _ui->roomView->GetGrid();
  QPoint mousePos(x, y);
  if (_snapToGrid) mousePos = QPoint(RoundNum(x, g.horSpacing), RoundNum(y, g.vertSpacing));
  _ui->roomView->mousePos = mousePos;
  _cursorPositionLabel->setText(tr("X %0, Y %1").arg(mousePos.x()).arg(mousePos.y()));
  if (_draggingPoint) {
    _pointsModel->SetData(_ui->roomView->mousePos.x(), _ui->roomView->selectedPointIndex, Path::Point::kXFieldNumber);
    _pointsModel->SetData(_ui->roomView->mousePos.y(), _ui->roomView->selectedPointIndex, Path::Point::kYFieldNumber);
  } else {
    _ui->pathPreviewBackground->update();  // manually call update to redraw cursor if no data changes
  }
}

void PathEditor::MousePressed(Qt::MouseButton button) {
  if (button == Qt::MouseButton::LeftButton) {
    _draggingPoint = true;
    // Check for point at location (reverse loop to get last added one if dups)
    for (int i = _pointsModel->rowCount() - 1; i >= 0; i--) {
      QPoint pt = _ui->roomView->Point(i);
      if (pt == _ui->roomView->mousePos) {
        QModelIndex newSelectIndex = _pointsModel->index(i, Path::Point::kXFieldNumber);
        _ui->pointsTableView->setCurrentIndex(newSelectIndex);
        _ui->pointsTableView->selectionModel()->select(newSelectIndex,
                                                       QItemSelectionModel::QItemSelectionModel::ClearAndSelect);
        return;
      }
    }
    // No point found. Add one and select it
    InsertPoint(_pointsModel->rowCount(), _ui->roomView->mousePos.x(), _ui->roomView->mousePos.y(),
                _ui->speedSpinBox->value());
  }
}

void PathEditor::MouseReleased(Qt::MouseButton button) {
  if (button == Qt::MouseButton::LeftButton) {
    _draggingPoint = false;
  }
}

void PathEditor::UpdateSelection(const QItemSelection& selected, const QItemSelection& /*deselected*/) {
  int selectIndex = -1;
  if (!selected.indexes().empty()) selectIndex = selected.indexes()[0].row();
  _ui->roomView->selectedPointIndex = selectIndex;
  _ui->pathPreviewBackground->update();
  _ui->deletePointButton->setDisabled((selectIndex == -1));
}

void PathEditor::on_addPointButton_pressed() {
  InsertPoint(_pointsModel->rowCount(), _ui->xSpinBox->value(), _ui->yspinBox->value(), _ui->speedSpinBox->value());
}

void PathEditor::on_insertPointButton_pressed() {
  int insertIndex = _ui->pointsTableView->selectionModel()->currentIndex().row();
  if (insertIndex < 0) insertIndex = 0;
  InsertPoint(insertIndex, _ui->xSpinBox->value(), _ui->yspinBox->value(), _ui->speedSpinBox->value());
}

void PathEditor::on_deletePointButton_pressed() {
  int deleteIndex = _ui->pointsTableView->selectionModel()->currentIndex().row();
  {
    RepeatedMessageModel::RowRemovalOperation remover(_pointsModel);
    remover.RemoveRow(deleteIndex);
  }

  if (_pointsModel->rowCount() > 0) {
    QModelIndex newSelectIndex =
        _pointsModel->index((deleteIndex == 0) ? 0 : deleteIndex - 1, Path::Point::kXFieldNumber);
    _ui->pointsTableView->setCurrentIndex(newSelectIndex);
    _ui->pointsTableView->selectionModel()->select(newSelectIndex,
                                                   QItemSelectionModel::QItemSelectionModel::ClearAndSelect);
  } else {
    _ui->deletePointButton->setDisabled(true);
    _ui->pointsTableView->selectionModel()->clearSelection();
    _ui->pathPreviewBackground->update();
  }
}
