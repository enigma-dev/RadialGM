#include "BackgroundEditor.h"

#include "ui_AddImageDialog.h"
#include "ui_BackgroundEditor.h"

#include "Components/Utility.h"

#include "codegen/Background.pb.h"

#include "gmx.h"

#include <QDesktopServices>
#include <QPainter>

using buffers::resources::Background;

BackgroundEditor::BackgroundEditor(ProtoModel* model, QWidget* parent)
    : BaseEditor(model, parent), ui(new Ui::BackgroundEditor) {
  ui->setupUi(this);

  ui->backgroundView->SetResourceModel(resMapper->GetModel());

  resMapper->addMapping(ui->smoothCheckBox, Background::kSmoothEdgesFieldNumber);
  resMapper->addMapping(ui->preloadCheckBox, Background::kPreloadFieldNumber);
  resMapper->addMapping(ui->tilesetGroupBox, Background::kUseAsTilesetFieldNumber);
  resMapper->addMapping(ui->tileWidthSpinBox, Background::kTileWidthFieldNumber);
  resMapper->addMapping(ui->tileHeightSpinBox, Background::kTileHeightFieldNumber);
  resMapper->addMapping(ui->horizontalOffsetSpinBox, Background::kHorizontalOffsetFieldNumber);
  resMapper->addMapping(ui->verticalOffsetSpinBox, Background::kVerticalOffsetFieldNumber);
  resMapper->addMapping(ui->horizontalSpacingSpinBox, Background::kHorizontalSpacingFieldNumber);
  resMapper->addMapping(ui->verticalSpacingSpinBox, Background::kVerticalSpacingFieldNumber);
  resMapper->toFirst();
}

BackgroundEditor::~BackgroundEditor() { delete ui; }

void BackgroundEditor::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVariant& oldValue,
                                   const QVector<int>& roles) {
  BaseEditor::dataChanged(topLeft, bottomRight, oldValue, roles);
  ui->backgroundView->update();
}

void BackgroundEditor::on_actionSave_triggered() {
  resMapper->SetDirty(false);
  this->parentWidget()->close();
}

void BackgroundEditor::on_actionZoomIn_triggered() {
  ui->backgroundView->SetZoom(ui->backgroundView->GetZoom() * 2);
}

void BackgroundEditor::on_actionZoomOut_triggered() {
  ui->backgroundView->SetZoom(ui->backgroundView->GetZoom() / 2);
}

void BackgroundEditor::on_actionZoom_triggered() { ui->backgroundView->SetZoom(1); }

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
  ui->backgroundView->SetImage(img);
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
      if (!ui->backgroundView->SetImage(newImage)) SetModelData(Background::kImageFieldNumber, lastImage);
    } else {
      if (ui->backgroundView->SetImage(fName)) SetModelData(Background::kImageFieldNumber, fName);
    }
  }
}

void BackgroundEditor::on_actionSaveImage_triggered() {
  ImageDialog* dialog = new ImageDialog(this, "background", true);
  dialog->exec();

  if (dialog->selectedFiles().size() > 0) {
    QString fName = dialog->selectedFiles()[0];
    ui->backgroundView->WriteImage(fName, dialog->selectedMimeTypeFilter());
  }
}

void BackgroundEditor::on_actionEditImage_triggered() {
  QString fName = GetModelData(Background::kImageFieldNumber).toString();
  QDesktopServices::openUrl(fName);  //TODO: file watcher reload
}
