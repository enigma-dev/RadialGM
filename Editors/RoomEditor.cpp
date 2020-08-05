#include "RoomEditor.h"
#include "Components/ArtManager.h"
#include "Components/QMenuView.h"
#include "MainWindow.h"
#include "ui_RoomEditor.h"

#include "Models/ImmediateMapper.h"
#include "Models/ProtoModel.h"
#include "Models/TreeSortFilterProxyModel.h"
#include "Room.pb.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMouseEvent>

#include <algorithm>

using View = buffers::resources::Room::View;

RoomEditor::RoomEditor(ProtoModel* model, const QPersistentModelIndex& root, QWidget* parent) :
    BaseEditor(model, root, parent), _ui(new Ui::RoomEditor) {
  _ui->setupUi(this);
  connect(_ui->actionSave, &QAction::triggered, this, &BaseEditor::OnSave);

  _ui->roomPreviewBackground->SetAssetView(_ui->roomView);

  _nodeMapper->addMapping(_ui->roomName, TreeNode::kNameFieldNumber);
  _nodeMapper->toFirst();

  _resMapper->addMapping(_ui->speedSpinBox, Room::kSpeedFieldNumber);
  _resMapper->addMapping(_ui->widthSpinBox, Room::kWidthFieldNumber);
  _resMapper->addMapping(_ui->heightSpinBox, Room::kHeightFieldNumber);
  _resMapper->addMapping(_ui->clearCheckBox, Room::kClearDisplayBufferFieldNumber);
  _resMapper->addMapping(_ui->persistentCheckBox, Room::kPersistentFieldNumber);
  _resMapper->addMapping(_ui->captionLineEdit, Room::kCaptionFieldNumber);

  _resMapper->addMapping(_ui->enableViewsCheckBox, Room::kEnableViewsFieldNumber);
  _resMapper->addMapping(_ui->clearViewportCheckBox, Room::kClearViewBackgroundFieldNumber);
  _resMapper->toFirst();

  _viewMapper = new ImmediateDataWidgetMapper(this);
  _viewMapper->addMapping(_ui->viewVisibleCheckBox, View::kVisibleFieldNumber);

  _viewMapper->addMapping(_ui->cameraXSpinBox, View::kXviewFieldNumber);
  _viewMapper->addMapping(_ui->cameraYSpinBox, View::kYviewFieldNumber);
  _viewMapper->addMapping(_ui->cameraWidthSpinBox, View::kWviewFieldNumber);
  _viewMapper->addMapping(_ui->cameraHeightSpinBox, View::kHviewFieldNumber);

  _viewMapper->addMapping(_ui->viewportXSpinBox, View::kXportFieldNumber);
  _viewMapper->addMapping(_ui->viewportYSpinBox, View::kYportFieldNumber);
  _viewMapper->addMapping(_ui->viewportWidthSpinBox, View::kWportFieldNumber);
  _viewMapper->addMapping(_ui->viewportHeightSpinBox, View::kHportFieldNumber);

  _viewMapper->addMapping(_ui->followingHBorderSpinBox, View::kHborderFieldNumber);
  _viewMapper->addMapping(_ui->followingVBorderSpinBox, View::kVborderFieldNumber);
  _viewMapper->addMapping(_ui->followingHSpeedSpinBox, View::kHspeedFieldNumber);
  _viewMapper->addMapping(_ui->followingVSpeedSpinBox, View::kVspeedFieldNumber);
  _viewMapper->toFirst();

  QMenuView* objMenu = new QMenuView(this);
  TreeSortFilterProxyModel* treeProxy = new TreeSortFilterProxyModel(this);
  treeProxy->SetFilterType(TreeNode::TypeCase::kObject);
  treeProxy->setSourceModel(MainWindow::treeModel.get());
  objMenu->setModel(treeProxy);
  _ui->objectSelectButton->setMenu(objMenu);

  connect(objMenu, &QMenu::triggered, this, &RoomEditor::SelectedObjectChanged);

  connect(_ui->currentViewComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          [=](int index) { _viewMapper->setCurrentIndex(index); });

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
