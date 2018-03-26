#include "BackgroundEditor.h"

#include "ui_AddImageDialog.h"
#include "ui_BackgroundEditor.h"

#include "Components/Utility.h"

#include "resources/Background.pb.h"

#include "gmx.h"

#include <QDesktopServices>
#include <QPainter>

using buffers::resources::Background;

BackgroundEditor::BackgroundEditor(QWidget* parent, ProtoModel* model)
    : BaseEditor(parent, model), ui(new Ui::BackgroundEditor) {
  ui->setupUi(this);

  ui->backgroundRenderer->SetResourceModel(model);

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
}

BackgroundEditor::~BackgroundEditor() { delete ui; }

void BackgroundEditor::dataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/,
                                   const QVector<int>& /*roles*/) {
  ui->backgroundRenderer->update();
}

void BackgroundEditor::on_actionSave_triggered() {
  model->SetDirty(false);
  this->parentWidget()->close();
}

void BackgroundEditor::on_actionZoomIn_triggered() {
  ui->backgroundRenderer->SetZoom(ui->backgroundRenderer->GetZoom() * 2);
}

void BackgroundEditor::on_actionZoomOut_triggered() {
  ui->backgroundRenderer->SetZoom(ui->backgroundRenderer->GetZoom() / 2);
}

void BackgroundEditor::on_actionZoom_triggered() { ui->backgroundRenderer->SetZoom(1); }

void BackgroundEditor::on_actionNewImage_triggered() {
  QDialog dialog(this);
  dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
  dialog.setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
  Ui::AddImageDialog dialogUI;
  dialogUI.setupUi(&dialog);
  dialog.setFixedSize(dialog.size());
  auto result = dialog.exec();
  if (result != QDialog::Accepted) return;
  QPixmap img(dialogUI.widthSpinBox->value(), dialogUI.heightSpinBox->value());
  img.fill(Qt::transparent);
  ui->backgroundRenderer->SetImage(img);
}

void BackgroundEditor::on_actionLoadImage_triggered() {
  ImageDialog* dialog = new ImageDialog(this, "background");
  dialog->exec();

  if (dialog->selectedFiles().size() > 0) {
    QString fName = dialog->selectedFiles()[0];
    Background* bkg = gmx::LoadBackground(fName.toStdString());
    if (bkg != nullptr) {
      QString lastImage = GetModelData(Background::kImageFieldNumber).toString();
      ReplaceBuffer(bkg);
      QString newImage = GetModelData(Background::kImageFieldNumber).toString();
      if (!ui->backgroundRenderer->SetImage(newImage)) SetModelData(Background::kImageFieldNumber, lastImage);
    } else {
      if (ui->backgroundRenderer->SetImage(fName)) SetModelData(Background::kImageFieldNumber, fName);
    }
  }
}

void BackgroundEditor::on_actionSaveImage_triggered() {
  ImageDialog* dialog = new ImageDialog(this, "background", true);
  dialog->exec();

  if (dialog->selectedFiles().size() > 0) {
    QString fName = dialog->selectedFiles()[0];
    ui->backgroundRenderer->WriteImage(fName, dialog->selectedMimeTypeFilter());
  }
}

void BackgroundEditor::on_actionEditImage_triggered() {
  QString fName = GetModelData(Background::kImageFieldNumber).toString();
  QDesktopServices::openUrl(fName);  //TODO: file watcher reload
}
