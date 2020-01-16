#include "RoomEditor.h"
#include "Components/ArtManager.h"
#include "Components/QMenuView.h"
#include "MainWindow.h"
#include "ui_RoomEditor.h"

#include "Models/ImmediateMapper.h"
#include "Models/MessageModel.h"
#include "Models/RepeatedMessageModel.h"
#include "Models/TreeSortFilterProxyModel.h"
#include "Room.pb.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMouseEvent>

#include <algorithm>

using View = buffers::resources::Room::View;

RoomEditor::RoomEditor(MessageModel* model, QWidget* parent) : BaseEditor(model, parent), _ui(new Ui::RoomEditor) {
  _ui->setupUi(this);
  connect(_ui->actionSave, &QAction::triggered, this, &BaseEditor::OnSave);

  _ui->roomPreviewBackground->SetAssetView(_ui->roomView);

  nodeMapper->addMapping(_ui->roomName, TreeNode::kNameFieldNumber);
  nodeMapper->toFirst();

  resMapper->addMapping(_ui->speedSpinBox, Room::kSpeedFieldNumber);
  resMapper->addMapping(_ui->widthSpinBox, Room::kWidthFieldNumber);
  resMapper->addMapping(_ui->heightSpinBox, Room::kHeightFieldNumber);
  resMapper->addMapping(_ui->clearCheckBox, Room::kClearDisplayBufferFieldNumber);
  resMapper->addMapping(_ui->persistentCheckBox, Room::kPersistentFieldNumber);
  resMapper->addMapping(_ui->captionLineEdit, Room::kCaptionFieldNumber);

  resMapper->addMapping(_ui->enableViewsCheckBox, Room::kEnableViewsFieldNumber);
  resMapper->addMapping(_ui->clearViewportCheckBox, Room::kClearViewBackgroundFieldNumber);
  resMapper->toFirst();

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

  RebindSubModels();
}

RoomEditor::~RoomEditor() { delete _ui; }

void RoomEditor::RebindSubModels() {
  _roomModel = _model->GetSubModel<MessageModel*>(TreeNode::kRoomFieldNumber);
  _ui->roomView->SetResourceModel(_roomModel);

  RepeatedMessageModel* im = _roomModel->GetSubModel<RepeatedMessageModel*>(Room::kInstancesFieldNumber);
  QSortFilterProxyModel* imp = new QSortFilterProxyModel(this);
  imp->setSourceModel(im);
  _ui->instancesListView->setModel(imp);

  for (int c = 0; c < im->columnCount(); ++c) {
    if (c != Room::Instance::kNameFieldNumber && c != Room::Instance::kObjectTypeFieldNumber &&
        c != Room::Instance::kIdFieldNumber)
      _ui->instancesListView->hideColumn(c);
    else
      _ui->instancesListView->resizeColumnToContents(c);
  }

  _ui->instancesListView->header()->swapSections(Room::Instance::kNameFieldNumber,
                                                 Room::Instance::kObjectTypeFieldNumber);

  RepeatedMessageModel* tm = _roomModel->GetSubModel<RepeatedMessageModel*>(Room::kTilesFieldNumber);
  QSortFilterProxyModel* tmp = new QSortFilterProxyModel(this);
  tmp->setSourceModel(tm);
  _ui->tilesListView->setModel(tmp);

  for (int c = 0; c < tm->columnCount(); ++c) {
    if (c != Room::Tile::kBackgroundNameFieldNumber && c != Room::Tile::kIdFieldNumber &&
        c != Room::Tile::kDepthFieldNumber && c != Room::Tile::kNameFieldNumber)
      _ui->tilesListView->hideColumn(c);
    else
      _ui->tilesListView->resizeColumnToContents(c);
  }

  _ui->tilesListView->header()->swapSections(Room::Tile::kNameFieldNumber, Room::Tile::kBackgroundNameFieldNumber);

  RepeatedMessageModel* vm = _roomModel->GetSubModel<RepeatedMessageModel*>(Room::kViewsFieldNumber);
  _viewMapper->setModel(vm);

  connect(_ui->instancesListView->selectionModel(), &QItemSelectionModel::selectionChanged,
          [=](const QItemSelection& selected, const QItemSelection& /*deselected*/) {
            if (selected.empty()) return;
            RepeatedMessageModel* im = _roomModel->GetSubModel<RepeatedMessageModel*>(Room::kInstancesFieldNumber);
            _ui->tilesListView->clearSelection();
            auto selectedIndex = selected.indexes().first();
            auto currentInstanceModel = im->GetSubModel<MessageModel*>(selectedIndex.row());
            _ui->layersPropertiesView->setModel(currentInstanceModel);
          });

  connect(_ui->tilesListView->selectionModel(), &QItemSelectionModel::selectionChanged,
          [=](const QItemSelection& selected, const QItemSelection& /*deselected*/) {
            if (selected.empty()) return;
            RepeatedMessageModel* tm = _roomModel->GetSubModel<RepeatedMessageModel*>(Room::kTilesFieldNumber);
            _ui->instancesListView->clearSelection();
            auto selectedIndex = selected.indexes().first();
            auto currentInstanceModel = tm->GetSubModel<MessageModel*>(selectedIndex.row());
            _ui->layersPropertiesView->setModel(currentInstanceModel);
          });

  BaseEditor::RebindSubModels();
}

void RoomEditor::SelectedObjectChanged(QAction* action) { _ui->currentObject->setText(action->text()); }

void RoomEditor::updateCursorPositionLabel(const QPoint& pos) {
  this->cursorPositionLabel->setText(tr("X %0, Y %1").arg(pos.x()).arg(pos.y()));
}

void RoomEditor::on_actionZoomIn_triggered() { _ui->roomPreviewBackground->ZoomIn(); }

void RoomEditor::on_actionZoomOut_triggered() { _ui->roomPreviewBackground->ZoomOut(); }

void RoomEditor::on_actionZoom_triggered() { _ui->roomPreviewBackground->ResetZoom(); }
