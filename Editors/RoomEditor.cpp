#include "RoomEditor.h"
#include "Components/ArtManager.h"
#include "Components/QMenuView.h"
#include "MainWindow.h"
#include "ui_RoomEditor.h"

#include "Models/ImmediateMapper.h"
#include "Models/MessageModel.h"
#include "Models/RepeatedMessageModel.h"
#include "Models/TreeSortFilterProxyModel.h"
#include "Models/RepeatedSortFilterProxyModel.h"

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

  _nodeMapper->addMapping(_ui->roomName, TreeNode::kNameFieldNumber);
  _nodeMapper->toFirst();

  _resMapper->addMapping(_ui->speedSpinBox, EGMRoom::kSpeedFieldNumber);
  _resMapper->addMapping(_ui->widthSpinBox, EGMRoom::kWidthFieldNumber);
  _resMapper->addMapping(_ui->heightSpinBox, EGMRoom::kHeightFieldNumber);
  _resMapper->addMapping(_ui->clearCheckBox, EGMRoom::kClearDisplayBufferFieldNumber);
  _resMapper->addMapping(_ui->persistentCheckBox, EGMRoom::kPersistentFieldNumber);
  _resMapper->addMapping(_ui->captionLineEdit, EGMRoom::kCaptionFieldNumber);

  _resMapper->addMapping(_ui->enableViewsCheckBox, EGMRoom::kEnableViewsFieldNumber);
  _resMapper->addMapping(_ui->clearViewportCheckBox, EGMRoom::kClearViewBackgroundFieldNumber);
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
  treeProxy->setSourceModel(MainWindow::treeModel);
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

  // This updates all the model views in the event of a sprite is changed
  connect(MainWindow::resourceMap, &ResourceModelMap::DataChanged, this, [this]() {
    _ui->instancesListView->reset();
    _ui->tilesListView->reset();
    _ui->layersPropertiesView->reset();
  });

  RoomEditor::RebindSubModels();
}

RoomEditor::~RoomEditor() { delete _ui; }

void RoomEditor::RebindSubModels() {
  _roomModel = _model->GetSubModel<MessageModel*>(TreeNode::kRoomFieldNumber);
  _ui->roomView->SetResourceModel(_roomModel);

  RepeatedMessageModel* im = _roomModel->GetSubModel<RepeatedMessageModel*>(EGMRoom::kInstancesFieldNumber);
  RepeatedSortFilterProxyModel* imp = new RepeatedSortFilterProxyModel(this);
  imp->SetSourceModel(im);
  _ui->instancesListView->setModel(imp);

  for (int c = 0; c < im->columnCount(); ++c) {
    if (c != im->FieldToColumn(EGMRoom::Instance::kNameFieldNumber) &&
        c != im->FieldToColumn(EGMRoom::Instance::kObjectTypeFieldNumber) &&
        c != im->FieldToColumn(EGMRoom::Instance::kIdFieldNumber))
      _ui->instancesListView->hideColumn(c);
    else
      _ui->instancesListView->resizeColumnToContents(c);
  }

  _ui->instancesListView->header()->swapSections(im->FieldToColumn(EGMRoom::Instance::kNameFieldNumber),
                                                 im->FieldToColumn(EGMRoom::Instance::kObjectTypeFieldNumber));

  RepeatedMessageModel* tm = _roomModel->GetSubModel<RepeatedMessageModel*>(EGMRoom::kTilesFieldNumber);
  RepeatedSortFilterProxyModel* tmp = new RepeatedSortFilterProxyModel(this);
  tmp->SetSourceModel(tm);
  _ui->tilesListView->setModel(tmp);

  for (int c = 0; c < tm->columnCount(); ++c) {
    if (c != tm->FieldToColumn(EGMRoom::Tile::kBackgroundNameFieldNumber) &&
        c != tm->FieldToColumn(EGMRoom::Tile::kIdFieldNumber) && c != tm->FieldToColumn(EGMRoom::Tile::kDepthFieldNumber) &&
        c != tm->FieldToColumn(EGMRoom::Tile::kNameFieldNumber))
      _ui->tilesListView->hideColumn(c);
    else
      _ui->tilesListView->resizeColumnToContents(c);
  }

  _ui->tilesListView->header()->swapSections(tm->FieldToColumn(EGMRoom::Tile::kNameFieldNumber),
                                             tm->FieldToColumn(EGMRoom::Tile::kBackgroundNameFieldNumber));

  RepeatedMessageModel* vm = _roomModel->GetSubModel<RepeatedMessageModel*>(EGMRoom::kViewsFieldNumber);
  _viewMapper->setModel(vm);

  connect(_ui->instancesListView->selectionModel(), &QItemSelectionModel::selectionChanged,
          [=](const QItemSelection& selected, const QItemSelection& /*deselected*/) {
            if (selected.empty()) return;
            _ui->tilesListView->clearSelection();
            auto selectedIndex = selected.indexes().first();
            auto currentInstanceModel = imp->GetSubModel(selectedIndex.row());
            _ui->layersPropertiesView->setModel(currentInstanceModel);
          });

  connect(_ui->tilesListView->selectionModel(), &QItemSelectionModel::selectionChanged,
          [=](const QItemSelection& selected, const QItemSelection& /*deselected*/) {
            if (selected.empty()) return;
            _ui->instancesListView->clearSelection();
            auto selectedIndex = selected.indexes().first();
            auto currentInstanceModel = tmp->GetSubModel(selectedIndex.row());
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

void RoomEditor::on_actionShowHideGrid_triggered() {
  _ui->roomPreviewBackground->SetGridVisible(_ui->actionShowHideGrid->isChecked());
}
