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
  connect(ui->actionSave, &QAction::triggered, this, &BaseEditor::OnSave);

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

  connect(roomButton, &QToolButton::pressed, [=]() {
    roomLineEdit->setText("");
    ui->roomView->SetResourceModel(nullptr);
  });

  connect(roomButton->menu, &QMenu::triggered, this, [=](QAction* action) {
    roomLineEdit->setText(action->text());
    ui->roomView->SetResourceModel(MainWindow::resourceMap->GetResourceByName(TypeCase::kRoom, action->text())
                                       ->GetSubModel(TreeNode::kRoomFieldNumber));
    ui->pathPreviewBackground->SetZoom(1);
  });

  ui->mainToolBar->addSeparator();

  QToolButton* zoom(new QToolButton(this));
  zoom->setIcon(ArtManager::GetIcon(":/actions/zoom.png"));
  ui->mainToolBar->addWidget(zoom);
  connect(zoom, &QToolButton::pressed, ui->pathPreviewBackground, [=]() { ui->pathPreviewBackground->SetZoom(1); });

  QToolButton* zoomIn(new QToolButton(this));
  zoomIn->setIcon(ArtManager::GetIcon(":/actions/zoom-in.png"));
  ui->mainToolBar->addWidget(zoomIn);
  connect(zoomIn, &QToolButton::pressed, ui->pathPreviewBackground,
          [=]() { ui->pathPreviewBackground->SetZoom(ui->pathPreviewBackground->GetZoom() * 2); });

  QToolButton* zoomOut(new QToolButton(this));
  zoomOut->setIcon(ArtManager::GetIcon(":/actions/zoom-out.png"));
  ui->mainToolBar->addWidget(zoomOut);
  connect(zoomOut, &QToolButton::pressed, ui->pathPreviewBackground,
          [=]() { ui->pathPreviewBackground->SetZoom(ui->pathPreviewBackground->GetZoom() / 2); });

  connect(ui->actionShowGrid, &QAction::triggered, [=]() {
    ui->roomView->GetGrid().show = !ui->roomView->GetGrid().show;
    ui->pathPreviewBackground->update();
  });

  connect(ui->actionSnaptoGrid, &QAction::triggered, [this]() { this->snapToGrid = !this->snapToGrid; });

  cursorPositionLabel = new QLabel();

  ui->statusBar->addWidget(cursorPositionLabel);

  ui->pointsTableView->installEventFilter(this);

  connect(xSnap, QOverload<int>::of(&QSpinBox::valueChanged),
          [this](int val) { ui->roomView->GetGrid().horSpacing = val; });

  connect(ySnap, QOverload<int>::of(&QSpinBox::valueChanged),
          [this](int val) { ui->roomView->GetGrid().vertSpacing = val; });

  connect(this, &BaseEditor::FocusGained, [=]() { ui->pathPreviewBackground->parentHasFocus = true; });
  connect(this, &BaseEditor::FocusLost, [=]() { ui->pathPreviewBackground->parentHasFocus = false; });

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

  connect(ui->pathPreviewBackground, &AssetScrollAreaBackground::MouseMoved, [=](int x, int y) {
    const GridDimensions g = ui->roomView->GetGrid();
    QPoint mousePos(x, y);
    if (snapToGrid) mousePos = QPoint(RoundNum(x, g.horSpacing), RoundNum(y, g.vertSpacing));
    ui->roomView->mousePos = mousePos;
    cursorPositionLabel->setText(tr("X %0, Y %1").arg(mousePos.x()).arg(mousePos.y()));
    if (draggingPoint) {
      pointsModel->setData(ui->roomView->selectedPointIndex, Path::Point::kXFieldNumber, ui->roomView->mousePos.x());
      pointsModel->setData(ui->roomView->selectedPointIndex, Path::Point::kYFieldNumber, ui->roomView->mousePos.y());
    } else
      ui->pathPreviewBackground->update();  // manually call update to redraw cursor if no data changes
  });

  connect(ui->pathPreviewBackground, &AssetScrollAreaBackground::MousePressed, [=](Qt::MouseButton button) {
    if (button == Qt::MouseButton::LeftButton) {
      draggingPoint = true;
      // Check for point at location (reverse loop to get last added one if dups)
      for (int i = pointsModel->rowCount(); i >= 0; i--) {
        QPoint pt(pointsModel->data(i, Path::Point::kXFieldNumber).toInt(),
                  pointsModel->data(i, Path::Point::kYFieldNumber).toInt());
        if (pt == ui->roomView->mousePos) {
          ui->roomView->selectedPointIndex = i;
          ui->pointsTableView->selectionModel()->select(pointsModel->index(i, Path::Point::kXFieldNumber),
                                                        QItemSelectionModel::QItemSelectionModel::ClearAndSelect);
          return;
        }
      }
      // No point found. Add one and select it
      InsertPoint(pointsModel->rowCount(), ui->roomView->mousePos.x(), ui->roomView->mousePos.y(),
                  ui->speedSpinBox->value());
    }
  });

  connect(ui->pathPreviewBackground, &AssetScrollAreaBackground::MouseReleased, [=](Qt::MouseButton button) {
    if (button == Qt::MouseButton::LeftButton) {
      draggingPoint = false;
    }
  });

  connect(ui->pointsTableView->selectionModel(), &QItemSelectionModel::selectionChanged, [this]() {
    int selectIndex = -1;
    if (ui->pointsTableView->selectionModel()->hasSelection())
      selectIndex = ui->pointsTableView->selectionModel()->currentIndex().row();
    ui->roomView->selectedPointIndex = selectIndex;
    ui->pathPreviewBackground->update();
    if (selectIndex != -1) ui->deletePointButton->setDisabled(false);
  });

  ui->deletePointButton->setDisabled(true);

  BaseEditor::RebindSubModels();
}

bool PathEditor::eventFilter(QObject* obj, QEvent* event) {
  // Resize columns to view size
  if (obj == ui->pointsTableView && event->type() == QEvent::Resize) {
    QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
    for (int c = 0; c < 3; ++c) {
      // Note: I ha nfi what 40 is here. Something todo with margins prolly
      ui->pointsTableView->setColumnWidth(c, (resizeEvent->size().width() - 40) / 3);
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

  qDebug() << deleteIndex;

  if (pointsModel->rowCount() > 0)
    ui->pointsTableView->selectionModel()->select(
        pointsModel->index((deleteIndex == 0) ? 0 : deleteIndex - 1, Path::Point::kXFieldNumber),
        QItemSelectionModel::QItemSelectionModel::ClearAndSelect);
  else
    ui->deletePointButton->setDisabled(true);
}
