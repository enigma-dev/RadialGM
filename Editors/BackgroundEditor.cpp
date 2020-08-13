#include "BackgroundEditor.h"

#include "ui_AddImageDialog.h"
#include "ui_BackgroundEditor.h"

#include "Components/Utility.h"

#include "Background.pb.h"

#include "gmx.h"

#include <QDebug>
#include <QDesktopServices>
#include <QPainter>

using buffers::resources::Background;

BackgroundEditor::BackgroundEditor(MessageModel* model, QWidget* parent)
    : BaseEditor(model, parent), _ui(new Ui::BackgroundEditor) {
  _ui->setupUi(this);

  connect(_ui->actionSave, &QAction::triggered, this, &BaseEditor::OnSave);

  //TODO: _mapper->mapName(_ui->nameEdit);
  _mapper->pushResource();

  _mapper->mapField(Background::kSmoothEdgesFieldNumber, _ui->smoothCheckBox);
  _mapper->mapField(Background::kPreloadFieldNumber, _ui->preloadCheckBox);
  _mapper->mapField(Background::kUseAsTilesetFieldNumber, _ui->tilesetGroupBox);
  _mapper->mapField(Background::kTileWidthFieldNumber, _ui->tileWidthSpinBox);
  _mapper->mapField(Background::kTileHeightFieldNumber, _ui->tileHeightSpinBox);
  _mapper->mapField(Background::kHorizontalOffsetFieldNumber, _ui->horizontalOffsetSpinBox);
  _mapper->mapField(Background::kVerticalOffsetFieldNumber, _ui->verticalOffsetSpinBox);
  _mapper->mapField(Background::kHorizontalSpacingFieldNumber, _ui->horizontalSpacingSpinBox);
  _mapper->mapField(Background::kVerticalSpacingFieldNumber, _ui->verticalSpacingSpinBox);
  _mapper->mapField(Background::kImageFieldNumber, _ui->backgroundView);

  _mapper->load();

  RebindSubModels();
}

BackgroundEditor::~BackgroundEditor() { delete _ui; }

void BackgroundEditor::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVariant& oldValue,
                                   const QVector<int>& roles) {
  BaseEditor::dataChanged(topLeft, bottomRight, oldValue, roles);
  _ui->backgroundView->update();
}

void BackgroundEditor::RebindSubModels() {
  _backgroundModel = _model->GetSubModel<MessageModel*>(TreeNode::kBackgroundFieldNumber);
  _ui->imagePreviewBackground->SetAssetView(_ui->backgroundView);
  //TODO:_ui->backgroundView->SetResourceModel(_resMapper->GetModel());
  BaseEditor::RebindSubModels();
}

void BackgroundEditor::on_actionZoomIn_triggered() { _ui->imagePreviewBackground->ZoomIn(); }

void BackgroundEditor::on_actionZoomOut_triggered() { _ui->imagePreviewBackground->ZoomOut(); }

void BackgroundEditor::on_actionZoom_triggered() { _ui->imagePreviewBackground->ResetZoom(); }

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
  _ui->backgroundView->SetImage(img);
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
      } else {
        qDebug() << "Failed to load gmx Background";
      }
    } else {
      // TODO: Copy data into our egm
      _backgroundModel->SetData(fName, Background::kImageFieldNumber);
    }
  }
}

void BackgroundEditor::on_actionSaveImage_triggered() {
  FileDialog* dialog = new FileDialog(this, FileDialog_t::BackgroundSave, true);

  if (dialog->exec() && dialog->selectedFiles().size() > 0) {
    QString fName = dialog->selectedFiles()[0];
    _ui->backgroundView->WriteImage(fName, dialog->selectedMimeTypeFilter());
  }
}

void BackgroundEditor::on_actionEditImage_triggered() {
  QString fName = _backgroundModel->Data(Background::kImageFieldNumber).toString();
  QDesktopServices::openUrl(QUrl::fromLocalFile(fName));
  // TODO: file watcher reload
  // TODO: editor settings
}
