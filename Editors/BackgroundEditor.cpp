#include "BackgroundEditor.h"

#include "ui_AddImageDialog.h"
#include "ui_BackgroundEditor.h"

#include "Components/Utility.h"

#include "Background.pb.h"

#include "gmx.h"

#include <QDebug>
#include <QDesktopServices>
#include <QPainter>

#include "Models/ImmediateMapper.h"

using buffers::resources::Background;

BackgroundEditor::BackgroundEditor(EditorModel* model, QWidget* parent)
    : BaseEditor(model, parent), _ui(new Ui::BackgroundEditor) {
  _ui->setupUi(this);
  //TODO: WTF
  _ui->imagePreviewBackground->SetAssetView(_ui->backgroundView);

  connect(_ui->actionSave, &QAction::triggered, this, &BaseEditor::OnSave);

  connect(model, &EditorModel::dataChanged, [](const QModelIndex& first,
          const QModelIndex& last){
    qDebug() << "motherfucker";
  });

  ImmediateDataWidgetMapper *mapper = new ImmediateDataWidgetMapper(this);
  mapper->setOrientation(Qt::Vertical);
  mapper->setModel(model);
  mapper->addMapping(_ui->nameEdit, 13);//TreeNode::kNameFieldNumber);
  mapper->setRootIndex(model->index(5, 0, model->_protoRoot));
  mapper->addMapping(_ui->tilesetGroupBox, 3);
  mapper->setRootIndex(QModelIndex());
  mapper->toFirst();
  mapper->revert();
  qDebug() << mapper->mappedSection(_ui->nameEdit);
  //mapper->revert();

  //_resMapper->addMapping(_ui->nameEdit, TreeNode::kNameFieldNumber);
  //TODO: FIXME
  /*
  _resMapper->addMapping(_ui->smoothCheckBox, Background::kSmoothEdgesFieldNumber);
  _resMapper->addMapping(_ui->preloadCheckBox, Background::kPreloadFieldNumber);
  _resMapper->addMapping(_ui->tilesetGroupBox, Background::kUseAsTilesetFieldNumber);
  _resMapper->addMapping(_ui->tileWidthSpinBox, Background::kTileWidthFieldNumber);
  _resMapper->addMapping(_ui->tileHeightSpinBox, Background::kTileHeightFieldNumber);
  _resMapper->addMapping(_ui->horizontalOffsetSpinBox, Background::kHorizontalOffsetFieldNumber);
  _resMapper->addMapping(_ui->verticalOffsetSpinBox, Background::kVerticalOffsetFieldNumber);
  _resMapper->addMapping(_ui->horizontalSpacingSpinBox, Background::kHorizontalSpacingFieldNumber);
  _resMapper->addMapping(_ui->verticalSpacingSpinBox, Background::kVerticalSpacingFieldNumber);
  _resMapper->toFirst();*/
}

BackgroundEditor::~BackgroundEditor() { delete _ui; }

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

}

void BackgroundEditor::on_actionSaveImage_triggered() {
  FileDialog* dialog = new FileDialog(this, FileDialog_t::BackgroundSave, true);

  if (dialog->exec() && dialog->selectedFiles().size() > 0) {
    QString fName = dialog->selectedFiles()[0];
    _ui->backgroundView->WriteImage(fName, dialog->selectedMimeTypeFilter());
  }
}

void BackgroundEditor::on_actionEditImage_triggered() {

}
