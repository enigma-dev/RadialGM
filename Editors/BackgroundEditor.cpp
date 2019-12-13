#include "BackgroundEditor.h"

#include "ui_AddImageDialog.h"
#include "ui_BackgroundEditor.h"

#include "Components/Utility.h"

#include "Background.pb.h"

#include "gmx.h"

#include <QDesktopServices>
#include <QPainter>
#include <QDebug>

using buffers::resources::Background;

BackgroundEditor::BackgroundEditor(ProtoModelPtr model, QWidget* parent)
    : BaseEditor(model, parent), ui(new Ui::BackgroundEditor) {
  ui->setupUi(this);

  connect(ui->actionSave, &QAction::triggered, this, &BaseEditor::OnSave);

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
  FileDialog* dialog = new FileDialog(this, FileDialog_t::BackgroundLoad, false);

  if (dialog->exec() && dialog->selectedFiles().size() > 0) {
    QString fName = dialog->selectedFiles()[0];
    if (fName.endsWith("Background.gmx")) {
      Background* bkg = gmx::LoadBackground(fName.toStdString());
      if (bkg != nullptr) {
        // QString lastData = GetModelData(Background::kImageFieldNumber).toString();
        ReplaceBuffer(bkg);
        // QString newData = GetModelData(Background::kImageFieldNumber).toString();
        // TODO: Copy data into our egm and reset the path
        // SetModelData(Background::kImageFieldNumber, lastData);
      } else qDebug() << "Failed to load gmx Background";
    } else {
      // TODO: Copy data into our egm
      SetModelData(Background::kImageFieldNumber, fName);
    }
  }
}

void BackgroundEditor::on_actionSaveImage_triggered() {
  FileDialog* dialog = new FileDialog(this, FileDialog_t::BackgroundSave, true);

  if (dialog->exec() && dialog->selectedFiles().size() > 0) {
    QString fName = dialog->selectedFiles()[0];
    ui->backgroundView->WriteImage(fName, dialog->selectedMimeTypeFilter());
  }
}

void BackgroundEditor::on_actionEditImage_triggered() {
  QString fName = GetModelData(Background::kImageFieldNumber).toString();
  QDesktopServices::openUrl(QUrl::fromLocalFile(fName));
  // TODO: file watcher reload
  // TODO: editor settings
}
