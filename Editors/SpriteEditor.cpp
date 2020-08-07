#include "SpriteEditor.h"
#include "Components/Utility.h"

#include "ui_SpriteEditor.h"

#include "Sprite.pb.h"

#include <QCheckBox>
#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QImage>
#include <QImageReader>
#include <QItemSelection>
#include <QMessageBox>
#include <QUuid>

using buffers::resources::Sprite;

SpriteEditor::SpriteEditor(EditorModel* model, QWidget* parent)
    : BaseEditor(model, parent), _ui(new Ui::SpriteEditor) {
  _ui->setupUi(this);
  connect(_ui->actionSave, &QAction::triggered, this, &BaseEditor::OnSave);
  _ui->scrollAreaWidget->SetAssetView(_ui->subimagePreview);

  QLabel* bboxLabel = new QLabel(tr("Show BBox "));
  QCheckBox* showBBox = new QCheckBox(this);
  showBBox->setChecked(true);
  _ui->mainToolBar->addWidget(bboxLabel);
  _ui->mainToolBar->addWidget(showBBox);
  connect(showBBox, &QCheckBox::stateChanged, _ui->subimagePreview, &SpriteView::SetShowBBox);

  QLabel* originLabel = new QLabel(tr("Show Origin "));
  QCheckBox* showOrigin = new QCheckBox(this);
  showOrigin->setChecked(true);
  _ui->mainToolBar->addWidget(originLabel);
  _ui->mainToolBar->addWidget(showOrigin);
  connect(showOrigin, &QCheckBox::stateChanged, _ui->subimagePreview, &SpriteView::SetShowOrigin);

  _mapper->mapName(_ui->nameEdit);
  _mapper->pushResource();

  _mapper->mapField(Sprite::kOriginXFieldNumber, _ui->originXSpinBox);
  _mapper->mapField(Sprite::kOriginYFieldNumber, _ui->originYSpinBox);
  _mapper->mapField(Sprite::kShapeFieldNumber, _ui->collisionShapeGroupBox, "currentIndex");
  _mapper->mapField(Sprite::kBboxModeFieldNumber, _ui->bboxComboBox, "currentIndex");
  _mapper->mapField(Sprite::kBboxLeftFieldNumber, _ui->leftSpinBox);
  _mapper->mapField(Sprite::kBboxRightFieldNumber, _ui->rightSpinBox);
  _mapper->mapField(Sprite::kBboxTopFieldNumber, _ui->topSpinBox);
  _mapper->mapField(Sprite::kBboxBottomFieldNumber, _ui->bottomSpinBox);

  _mapper->load();
}

SpriteEditor::~SpriteEditor() { delete _ui; }

void SpriteEditor::LoadedMismatchedImage(QSize expectedSize, QSize actualSize) {
  QString expected =
      tr("Expected: ") + QString::number(expectedSize.width()) + " x " + QString::number(expectedSize.height());

  QString actual = tr("Actual: ") + QString::number(actualSize.width()) + " x " + QString::number(actualSize.height());

  QMessageBox::critical(this, tr("Failed to load image"),
                        QString(tr("Error mismatched image sizes\n%1\n%2")).arg(expected).arg(actual), QMessageBox::Ok);
  // TODO: Add some remedies to this such as streching, croping or scaling the background
}

void SpriteEditor::SubImagesRemoved() {

}

void SpriteEditor::RemoveSelectedIndexes() {

}

void SpriteEditor::SelectionChanged(const QItemSelection& selected, const QItemSelection& /*deselected*/) {
  if (!selected.empty()) {
    _ui->subimagePreview->SetSubimage(selected.indexes().back().row());
  }
}

void SpriteEditor::on_bboxComboBox_currentIndexChanged(int index) {

}

void SpriteEditor::on_actionNewSubimage_triggered() {

}

void SpriteEditor::on_actionDeleteSubimages_triggered() { RemoveSelectedIndexes(); }

void SpriteEditor::on_actionCut_triggered() {

}

void SpriteEditor::on_actionPaste_triggered() {

}

void SpriteEditor::on_actionCopy_triggered() {

}

void SpriteEditor::on_actionLoadSubimages_triggered() {

}

void SpriteEditor::on_actionAddSubimages_triggered() {

}

void SpriteEditor::on_actionZoom_triggered() { _ui->scrollAreaWidget->ResetZoom(); }

void SpriteEditor::on_actionZoomIn_triggered() { _ui->scrollAreaWidget->ZoomIn(); }

void SpriteEditor::on_actionZoomOut_triggered() { _ui->scrollAreaWidget->ZoomOut(); }

void SpriteEditor::on_actionEditSubimages_triggered() {

}

void SpriteEditor::on_centerOriginButton_clicked() {
  QSize sz = _ui->subimagePreview->GetPixmap().size();
  _ui->originXSpinBox->setValue(sz.width() / 2);
  _ui->originYSpinBox->setValue(sz.height() / 2);
}
