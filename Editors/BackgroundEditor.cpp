#include "BackgroundEditor.h"

#include "ui_AddImageDialog.h"
#include "ui_BackgroundEditor.h"

#include "Models/ProtoModel.h"
#include "resources/Background.pb.h"

#include <QPainter>

using buffers::resources::Background;

BackgroundEditor::BackgroundEditor(QWidget* parent, ResourceModel* model)
    : BaseEditor(parent, model), ui(new Ui::BackgroundEditor) {
  ui->setupUi(this);

  mapper->addMapping(ui->smoothCheckBox, Background::kSmoothEdgesFieldNumber);
  mapper->addMapping(ui->preloadCheckBox, Background::kPreloadFieldNumber);
  mapper->addMapping(ui->transparentCheckBox, Background::kTransparentFieldNumber);
  mapper->addMapping(ui->tilesetGroupBox, Background::kUseAsTilesetFieldNumber);
  mapper->addMapping(ui->tileWidthSpinBox, Background::kTileWidthFieldNumber);
  mapper->addMapping(ui->tileHeightSpinBox, Background::kTileHeightFieldNumber);
  mapper->addMapping(ui->horizontalOffsetSpinBox, Background::kHorizontalOffsetFieldNumber);
  mapper->addMapping(ui->verticalOffsetSpinBox, Background::kVerticalOffsetFieldNumber);
  mapper->addMapping(ui->horizontalSpacingSpinBox, Background::kHorizontalSpacingFieldNumber);
  mapper->addMapping(ui->verticalSpacingSpinBox, Background::kVerticalSpacingFieldNumber);
  mapper->toFirst();

  ui->backgroundRenderer->setTransparent(ui->transparentCheckBox->isChecked());
  ui->backgroundRenderer->setImage(QPixmap(GetModelData(Background::kImageFieldNumber).toString()));
  ui->backgroundRenderer->setGrid(ui->tilesetGroupBox->isChecked(), ui->horizontalOffsetSpinBox->value(),
                                  ui->verticalOffsetSpinBox->value(), ui->tileWidthSpinBox->value(),
                                  ui->tileHeightSpinBox->value(), ui->horizontalSpacingSpinBox->value(),
                                  ui->verticalSpacingSpinBox->value());
}

BackgroundEditor::~BackgroundEditor() { delete ui; }

void BackgroundEditor::dataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/,
                                   const QVector<int>& /*roles*/) {
  ui->backgroundRenderer->setTransparent(ui->transparentCheckBox->isChecked());
  ui->backgroundRenderer->setGrid(ui->tilesetGroupBox->isChecked(), ui->horizontalOffsetSpinBox->value(),
                                  ui->verticalOffsetSpinBox->value(), ui->tileWidthSpinBox->value(),
                                  ui->tileHeightSpinBox->value(), ui->horizontalSpacingSpinBox->value(),
                                  ui->verticalSpacingSpinBox->value());
}

void BackgroundEditor::on_actionSave_triggered() {
  ui->smoothCheckBox->setAcceptDrops(!ui->smoothCheckBox->acceptDrops());
}

void BackgroundEditor::on_actionZoomIn_triggered() {
  ui->backgroundRenderer->setZoom(ui->backgroundRenderer->getZoom() * 2);
}

void BackgroundEditor::on_actionZoomOut_triggered() {
  ui->backgroundRenderer->setZoom(ui->backgroundRenderer->getZoom() / 2);
}

void BackgroundEditor::on_actionZoom_triggered() { ui->backgroundRenderer->setZoom(1); }

void BackgroundEditor::on_actionNewImage_triggered() {
  QDialog* dialog = new QDialog(this);
  dialog->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
  dialog->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
  Ui::AddImageDialog dialogUI;
  dialogUI.setupUi(dialog);
  dialog->setFixedSize(dialog->size());
  auto result = dialog->exec();
  if (result != QDialog::Accepted) return;
  QPixmap img(dialogUI.widthSpinBox->value(), dialogUI.heightSpinBox->value());
  img.fill(Qt::transparent);
  ui->backgroundRenderer->setImage(img);
}
