#include "PathEditor.h"
#include "Components/ArtManager.h"
#include "Widgets/ResourceSelector.h"

#include "ui_PathEditor.h"

#include <QEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QSpinBox>
#include <QToolButton>

using buffers::resources::Path;

PathEditor::PathEditor(EditorModel* model, QWidget* parent) :
    BaseEditor(model, parent), _ui(new Ui::PathEditor) {
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

  _ui->pointsTableView->installEventFilter(this);

  connect(xSnap, QOverload<int>::of(&QSpinBox::valueChanged), _ui->pathPreviewBackground,
          &AssetScrollAreaBackground::SetGridHSnap);
  connect(ySnap, QOverload<int>::of(&QSpinBox::valueChanged), _ui->pathPreviewBackground,
          &AssetScrollAreaBackground::SetGridVSnap);

  _mapper->mapName(_ui->nameEdit);
  _mapper->pushResource();

  _mapper->mapField(Path::kSmoothFieldNumber, _ui->smoothCheckBox);
  _mapper->mapField(Path::kClosedFieldNumber, _ui->closedCheckBox);
  _mapper->mapField(Path::kPrecisionFieldNumber, _ui->precisionSpinBox);
  _mapper->mapField(Path::kHsnapFieldNumber, xSnap);
  _mapper->mapField(Path::kVsnapFieldNumber, ySnap);

  _mapper->load();
}

PathEditor::~PathEditor() { delete _ui; }

bool PathEditor::eventFilter(QObject* obj, QEvent* event) {

}

void PathEditor::InsertPoint(int index, int x, int y, int speed) {

}

void PathEditor::SetSnapToGrid(bool snap) { this->_snapToGrid = snap; }

void PathEditor::RoomMenuItemSelected(QAction* action) {

}

void PathEditor::RoomMenuButtonPressed() {

}

void PathEditor::MouseMoved(int x, int y) {

}

void PathEditor::MousePressed(Qt::MouseButton button) {

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

}

void PathEditor::on_insertPointButton_pressed() {
  int insertIndex = _ui->pointsTableView->selectionModel()->currentIndex().row();
  if (insertIndex < 0) insertIndex = 0;
  InsertPoint(insertIndex, _ui->xSpinBox->value(), _ui->yspinBox->value(), _ui->speedSpinBox->value());
}

void PathEditor::on_deletePointButton_pressed() {

}
