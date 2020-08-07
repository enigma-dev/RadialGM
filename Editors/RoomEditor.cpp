#include "RoomEditor.h"
#include "Components/ArtManager.h"
#include "Components/QMenuView.h"
#include "MainWindow.h"
#include "ui_RoomEditor.h"

#include "Models/TreeSortFilterProxyModel.h"
#include "Room.pb.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMouseEvent>

#include <algorithm>

using View = buffers::resources::Room::View;

RoomEditor::RoomEditor(EditorModel* model, QWidget* parent) :
    BaseEditor(model, parent), _ui(new Ui::RoomEditor) {
  _ui->setupUi(this);
  connect(_ui->actionSave, &QAction::triggered, this, &BaseEditor::OnSave);

  _ui->roomPreviewBackground->SetAssetView(_ui->roomView);

  _mapper->mapName(_ui->roomName);
  _mapper->pushResource();

  _mapper->mapField(Room::kSpeedFieldNumber, _ui->speedSpinBox);
  _mapper->mapField(Room::kWidthFieldNumber, _ui->widthSpinBox);
  _mapper->mapField(Room::kHeightFieldNumber, _ui->heightSpinBox);
  _mapper->mapField(Room::kClearDisplayBufferFieldNumber, _ui->clearCheckBox);
  _mapper->mapField(Room::kPersistentFieldNumber, _ui->persistentCheckBox);
  _mapper->mapField(Room::kCaptionFieldNumber, _ui->captionLineEdit);

  _mapper->mapField(Room::kEnableViewsFieldNumber, _ui->enableViewsCheckBox);
  _mapper->mapField(Room::kClearViewBackgroundFieldNumber, _ui->clearViewportCheckBox);

  //TODO: FIX GROUP
  _mapper->mapField(View::kVisibleFieldNumber, _ui->viewVisibleCheckBox);

  _mapper->mapField(View::kXviewFieldNumber, _ui->cameraXSpinBox);
  _mapper->mapField(View::kYviewFieldNumber, _ui->cameraYSpinBox);
  _mapper->mapField(View::kWviewFieldNumber, _ui->cameraWidthSpinBox);
  _mapper->mapField(View::kHviewFieldNumber, _ui->cameraHeightSpinBox);

  _mapper->mapField(View::kXportFieldNumber, _ui->viewportXSpinBox);
  _mapper->mapField(View::kYportFieldNumber, _ui->viewportYSpinBox);
  _mapper->mapField(View::kWportFieldNumber, _ui->viewportWidthSpinBox);
  _mapper->mapField(View::kHportFieldNumber, _ui->viewportHeightSpinBox);

  _mapper->mapField(View::kHborderFieldNumber, _ui->followingHBorderSpinBox);
  _mapper->mapField(View::kVborderFieldNumber, _ui->followingVBorderSpinBox);
  _mapper->mapField(View::kHspeedFieldNumber, _ui->followingHSpeedSpinBox);
  _mapper->mapField(View::kVspeedFieldNumber, _ui->followingVSpeedSpinBox);
  //_mapper->popField();

  _mapper->load();


  QMenuView* objMenu = new QMenuView(this);
  TreeSortFilterProxyModel* treeProxy = new TreeSortFilterProxyModel(this);
  treeProxy->SetFilterType(TreeNode::TypeCase::kObject);
  treeProxy->setSourceModel(MainWindow::treeModel.get());
  objMenu->setModel(treeProxy);
  _ui->objectSelectButton->setMenu(objMenu);

  connect(objMenu, &QMenu::triggered, this, &RoomEditor::SelectedObjectChanged);

  //TODO: FIXME
  //connect(_ui->currentViewComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          //[=](int index) { _viewMapper->setCurrentIndex(index); });

  cursorPositionLabel = new QLabel();
  connect(_ui->roomPreviewBackground, &AssetScrollAreaBackground::MouseMoved, [=](int x, int y) {
    const GridDimensions g = _ui->roomView->GetGrid();
    cursorPositionLabel->setText(tr("X %0, Y %1").arg(RoundNum(x, g.horSpacing)).arg(RoundNum(y, g.vertSpacing)));
  });
  _ui->statusBar->addWidget(cursorPositionLabel);

  _assetNameLabel = new QLabel("obj_xxx");
  _ui->statusBar->addWidget(_assetNameLabel);
}

RoomEditor::~RoomEditor() { delete _ui; }

void RoomEditor::SelectedObjectChanged(QAction* action) { _ui->currentObject->setText(action->text()); }

void RoomEditor::updateCursorPositionLabel(const QPoint& pos) {
  this->cursorPositionLabel->setText(tr("X %0, Y %1").arg(pos.x()).arg(pos.y()));
}

void RoomEditor::on_actionZoomIn_triggered() { _ui->roomPreviewBackground->ZoomIn(); }

void RoomEditor::on_actionZoomOut_triggered() { _ui->roomPreviewBackground->ZoomOut(); }

void RoomEditor::on_actionZoom_triggered() { _ui->roomPreviewBackground->ResetZoom(); }
