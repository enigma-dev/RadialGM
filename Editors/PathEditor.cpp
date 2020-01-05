#include "PathEditor.h"
#include "Components/ArtManager.h"
#include "Widgets/ResourceSelector.h"

#include "ui_PathEditor.h"

#include <QLineEdit>
#include <QMouseEvent>
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

  QLabel* ySnapLabel(new QLabel(tr("Y Snap"), this));
  ui->mainToolBar->addWidget(ySnapLabel);

  QSpinBox* ySnap(new QSpinBox(this));
  ui->mainToolBar->addWidget(ySnap);

  ui->mainToolBar->addSeparator();

  ResourceSelector* roomButton(new ResourceSelector(this, TreeNode::TypeCase::kRoom));
  roomButton->setPopupMode(QToolButton::MenuButtonPopup);
  roomButton->setIcon(ArtManager::GetIcon(":/resources/room.png"));
  ui->mainToolBar->addWidget(roomButton);

  QLineEdit* roomLineEdit(new QLineEdit(this));
  roomLineEdit->setReadOnly(true);
  roomLineEdit->setMaximumWidth(150);
  ui->mainToolBar->addWidget(roomLineEdit);

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

  cursorPositionLabel = new QLabel();
  connect(ui->pathPreviewBackground, &AssetScrollAreaBackground::MouseMoved, [=](int x, int y) {
    const GridDimensions g = ui->roomView->GetGrid();
    QPoint mousePos(RoundNum(x, g.horSpacing), RoundNum(y, g.vertSpacing));
    ui->roomView->mousePos = mousePos;
    cursorPositionLabel->setText(tr("X %0, Y %1").arg(mousePos.x()).arg(mousePos.y()));
    ui->pathPreviewBackground->update();
  });
  ui->statusBar->addWidget(cursorPositionLabel);

  ProtoModelPtr pathModel = _model->GetSubModel(TreeNode::kPathFieldNumber);
  ui->roomView->SetPathModel(pathModel);
  RepeatedProtoModelPtr pointsModel = pathModel->GetRepeatedSubModel(Path::kPointsFieldNumber);
  ui->pointsTableView->setModel(pointsModel);
  ui->pointsTableView->hideColumn(0);
}

PathEditor::~PathEditor() { delete ui; }
