#include "SpriteEditor.h"

#include "ui_SpriteEditor.h"

#include <QMessageBox>

SpriteEditor::SpriteEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::SpriteEditor) {
  ui->setupUi(this);

  spriteModel =
      new SpriteModel(static_cast<buffers::resources::Sprite*>(model->GetSubModel(TreeNode::kSpriteFieldNumber)->GetBuffer())->mutable_subimages(), this);

  connect(spriteModel, &SpriteModel::MismatchedImageSize, this, &SpriteEditor::LoadedMismatchedImage);
  ui->subImageList->setModel(spriteModel);
  ui->subImageList->setIconSize(spriteModel->GetIconSize());
}

SpriteEditor::~SpriteEditor() { delete ui; }

void SpriteEditor::LoadedMismatchedImage(QSize /*expectedSize*/, QSize /*actualSize*/) {
  QMessageBox::critical(this, tr("Failed to load image"), tr("Error mismatched size"), QMessageBox::Ok);
}

void SpriteEditor::on_actionSave_triggered()
{
    QWidget *parent = parentWidget();
    parent->close(); //pass just save in future
}
