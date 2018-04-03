#include "SpriteEditor.h"

#include "ui_SpriteEditor.h"

#include <QMessageBox>

SpriteEditor::SpriteEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::SpriteEditor) {
  ui->setupUi(this);

  spriteModel =
      new SpriteModel(static_cast<buffers::resources::Sprite*>(model->GetBuffer())->mutable_subimages(), this);
  //spriteModel->SetMinIconSize(32, 32);
  //spriteModel->SetMaxIconSize(32, 32);

  connect(spriteModel, &SpriteModel::MismatchedImageSize, this, &SpriteEditor::LoadedMismatedImage);
  ui->subImageList->setModel(spriteModel);
  ui->subImageList->setIconSize(spriteModel->GetIconSize());

  connect(ui->subImagePreview, &InfiniteGrid::MouseMoved, this, &SpriteEditor::MouseMoved);

  ui->subImagePreview->SetPixmap(spriteModel->data(spriteModel->index(0), SpriteRole::PixmapRole).value<QPixmap>());
}

SpriteEditor::~SpriteEditor() { delete ui; }

void SpriteEditor::MouseMoved(int x, int y) {
  ui->statusBar->showMessage("x: " + QString::number(x) + " " + "y: " + QString::number(y));
}

void SpriteEditor::LoadedMismatedImage(QSize /*expectedSize*/, QSize /*actualSize*/) {
  QMessageBox::critical(this, tr("Failed to load image"), tr("Error mismatched size"), QMessageBox::Ok);
}
