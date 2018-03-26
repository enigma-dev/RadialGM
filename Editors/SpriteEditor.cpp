#include "SpriteEditor.h"

#include "ui_SpriteEditor.h"

<<<<<<< HEAD
SpriteEditor::SpriteEditor(QWidget* parent, ProtoModel* model)
    : BaseEditor(parent, model), ui(new Ui::SpriteEditor) {
=======
#include <QMessageBox>

SpriteEditor::SpriteEditor(QWidget* parent, ProtoModel* model) : BaseEditor(parent, model), ui(new Ui::SpriteEditor) {
>>>>>>> SpriteEdit
  ui->setupUi(this);

  spriteModel =
      new SpriteModel(static_cast<buffers::resources::Sprite*>(model->GetBuffer())->mutable_subimages(), this);

  connect(spriteModel, &SpriteModel::MismatchedImageSize, this, &SpriteEditor::LoadedMismatedImage);
  ui->subImageList->setModel(spriteModel);
  ui->subImageList->setIconSize(spriteModel->GetIconSize());
}

SpriteEditor::~SpriteEditor() { delete ui; }

void SpriteEditor::LoadedMismatedImage(QSize /*expectedSize*/, QSize /*actualSize*/) {
  QMessageBox::critical(this, tr("Failed to load image"), tr("Error mismatched size"), QMessageBox::Ok);
}
