#include "ui_SpriteEditor.h"

#include "Components/Utility.h"
#include "SpriteEditor.h"
#include "Models/MessageModel.h"
#include "Utils/QBoilerplate.h"

#include <QCheckBox>
#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QImage>
#include <QImageReader>
#include <QItemSelection>
#include <QMessageBox>
#include <QUuid>

SpriteEditor::SpriteEditor(MessageModel* model, QWidget* parent)
    : BaseEditor(model, parent), _ui(new Ui::SpriteEditor) {
  _ui->setupUi(this);
  connect(_ui->actionSave, &QAction::triggered, this, &BaseEditor::OnSave);
  _ui->scrollAreaWidget->SetAssetView(_ui->subimagePreview);

  QCheckBox* showBBox = new QCheckBox(tr("Show BBox"), this);
  showBBox->setChecked(true);
  QCheckBox* showOrigin = new QCheckBox(tr("Show Origin"), this);
  showOrigin->setChecked(true);
  _ui->subImageList->setItemDelegate(new qboilerplate::RemoveDisplayRole);

  _ui->mainToolBar->addWidget(showBBox);
  _ui->mainToolBar->addWidget(showOrigin);
  connect(showBBox, &QCheckBox::stateChanged, _ui->subimagePreview, &SpriteView::SetShowBBox);
  connect(showOrigin, &QCheckBox::stateChanged, _ui->subimagePreview, &SpriteView::SetShowOrigin);

  _nodeMapper->addMapping(_ui->nameEdit, TreeNode::kNameFieldNumber);
  _resMapper->addMapping(_ui->originXSpinBox, Sprite::kOriginXFieldNumber);
  _resMapper->addMapping(_ui->originYSpinBox, Sprite::kOriginYFieldNumber);
  _resMapper->addMapping(_ui->collisionShapeGroupBox, Sprite::kShapeFieldNumber, "currentIndex");
  _resMapper->addMapping(_ui->bboxComboBox, Sprite::kBboxModeFieldNumber, "currentIndex");
  _resMapper->addMapping(_ui->leftSpinBox, Sprite::kBboxLeftFieldNumber);
  _resMapper->addMapping(_ui->rightSpinBox, Sprite::kBboxRightFieldNumber);
  _resMapper->addMapping(_ui->topSpinBox, Sprite::kBboxTopFieldNumber);
  _resMapper->addMapping(_ui->bottomSpinBox, Sprite::kBboxBottomFieldNumber);

  SpriteEditor::RebindSubModels();
}

SpriteEditor::~SpriteEditor() { delete _ui; }

void SpriteEditor::RebindSubModels() {
  _spriteModel = _model->GetSubModel<MessageModel*>(TreeNode::kSpriteFieldNumber);
  _subimagesModel = _spriteModel->GetSubModel<RepeatedStringModel*>(Sprite::kSubimagesFieldNumber);
  connect(_spriteModel, &ProtoModel::DataChanged, this, [this]() { _ui->subimagePreview->update(); });

  _ui->subImageList->setModel(_subimagesModel);

  _ui->subimagePreview->SetResourceModel(_spriteModel);
  //connect(_subimagesModel, &RepeatedImageModel::MismatchedImageSize, this, &SpriteEditor::LoadedMismatchedImage);

  connect(_ui->subImageList->selectionModel(), &QItemSelectionModel::selectionChanged, this,
          &SpriteEditor::SelectionChanged);

  connect(_subimagesModel, &QAbstractItemModel::modelReset, this, &SpriteEditor::SubImagesRemoved);

  BaseEditor::RebindSubModels();

  on_bboxComboBox_currentIndexChanged(_spriteModel->Data(FieldPath::Of<Sprite>(Sprite::kBboxModeFieldNumber)).toInt());
}

void SpriteEditor::LoadedMismatchedImage(QSize expectedSize, QSize actualSize) {
  QString expected =
      tr("Expected: ") + QString::number(expectedSize.width()) + " x " + QString::number(expectedSize.height());

  QString actual = tr("Actual: ") + QString::number(actualSize.width()) + " x " + QString::number(actualSize.height());

  QMessageBox::critical(this, tr("Failed to load image"),
                        QString(tr("Error mismatched image sizes\n%1\n%2")).arg(expected, actual), QMessageBox::Ok);
  // TODO: Add some remedies to this such as streching, croping or scaling the background
}

void SpriteEditor::SubImagesRemoved() {
  _ui->subimagePreview->SetSubimage((_subimagesModel->rowCount() == 0) ? -1 : 0);
}

void SpriteEditor::RemoveSelectedIndexes() {
  RepeatedStringModel::RowRemovalOperation remover(_subimagesModel);
  auto const idxs = _ui->subImageList->selectionModel()->selectedIndexes();
  for (QModelIndex idx : idxs) {
    remover.RemoveRow(idx.row());
  }
}

void SpriteEditor::SelectionChanged(const QItemSelection& selected, const QItemSelection& /*deselected*/) {
  if (!selected.empty()) {
    _ui->subimagePreview->SetSubimage(selected.indexes().back().row());
  }
}

void SpriteEditor::on_bboxComboBox_currentIndexChanged(int index) {
  bool manual = (index != Sprite::BoundingBox::Sprite_BoundingBox_MANUAL);
  _ui->leftSpinBox->setDisabled(manual);
  _ui->rightSpinBox->setDisabled(manual);
  _ui->topSpinBox->setDisabled(manual);
  _ui->bottomSpinBox->setDisabled(manual);

  if (index == Sprite::BoundingBox::Sprite_BoundingBox_AUTOMATIC) {
    QRectF rect = _ui->subimagePreview->AutomaticBBoxRect();
    _ui->leftSpinBox->setValue(rect.x());
    _ui->topSpinBox->setValue(rect.y());
    _ui->rightSpinBox->setValue(rect.x() + rect.width());
    _ui->bottomSpinBox->setValue(rect.y() + rect.height());
  } else if (index == Sprite::BoundingBox::Sprite_BoundingBox_FULL_IMAGE) {
    QPixmap p = _ui->subimagePreview->GetPixmap();
    _ui->leftSpinBox->setValue(0);
    _ui->topSpinBox->setValue(0);
    _ui->rightSpinBox->setValue(p.width());
    _ui->bottomSpinBox->setValue(p.height());
  }
}

void SpriteEditor::on_actionNewSubimage_triggered() {
  QSize imgSize;
  if (_subimagesModel->rowCount() == 0) {
    // TODO: Dialog to ask size
    imgSize = QSize(64, 64);
  } else {
    imgSize = _ui->subimagePreview->GetPixmap().size();
  }

  QImage img(imgSize, QImage::Format_RGBA8888);
  img.fill(Qt::red);
  // TODO: Generate real name and save in proper directory inside the EGM
  QString uid = QUuid::createUuid().toString();
  QString fName(QDir::tempPath() + "/" + uid.mid(1, uid.length() - 2) + ".png");
  img.save(fName);
  _subimagesModel->insertRow(_subimagesModel->rowCount());
  _subimagesModel->SetDataAtRow(_subimagesModel->rowCount() - 1, fName);
}

void SpriteEditor::on_actionDeleteSubimages_triggered() { RemoveSelectedIndexes(); }

void SpriteEditor::on_actionCut_triggered() {
  QGuiApplication::clipboard()->setMimeData(
      _subimagesModel->mimeData(_ui->subImageList->selectionModel()->selectedIndexes()));
  RemoveSelectedIndexes();
}

void SpriteEditor::on_actionPaste_triggered() {
  _subimagesModel->dropMimeData(QGuiApplication::clipboard()->mimeData(), Qt::DropAction::CopyAction,
                                _subimagesModel->rowCount(), 0, QModelIndex());
}

void SpriteEditor::on_actionCopy_triggered() {
  QGuiApplication::clipboard()->setMimeData(
      _subimagesModel->mimeData(_ui->subImageList->selectionModel()->selectedIndexes()));
}

void SpriteEditor::on_actionLoadSubimages_triggered() {
  FileDialog* dialog = new FileDialog(this, FileDialog_t::BackgroundLoad, false);
  dialog->setFileMode(QFileDialog::ExistingFiles);

  if (dialog->exec() && dialog->selectedFiles().size() > 0) {
    QImageReader img(dialog->selectedFiles().at(0));
    if (img.size().width() > 0 && img.size().height() > 0) {
      _subimagesModel->Clear();
      auto const selected = dialog->selectedFiles();
      for (const QString& fName : selected) {
        QImageReader newImg(fName);
        if (img.size() == newImg.size()) {
          _subimagesModel->insertRow(_subimagesModel->rowCount());
          // TODO: Internalize file
          _subimagesModel->SetDataAtRow(_subimagesModel->rowCount() - 1, fName);
          _ui->subimagePreview->SetSubimage(0);
          // Redo BBox
          on_bboxComboBox_currentIndexChanged(
              _spriteModel->Data(FieldPath::Of<Sprite>(Sprite::kBboxModeFieldNumber)).toInt());
        } else {
          LoadedMismatchedImage(img.size(), newImg.size());
        }
      }
    } else {
      qDebug() << " Failed to load image: " << dialog->selectedFiles().at(0);
    }
  }
}

// FIXME: this duplicated from above
void SpriteEditor::on_actionAddSubimages_triggered() {
  FileDialog* dialog = new FileDialog(this, FileDialog_t::SpriteLoad, false);
  dialog->setFileMode(QFileDialog::ExistingFiles);

  if (dialog->exec() && dialog->selectedFiles().size() > 0) {
    QSize imgSize = _ui->subimagePreview->GetPixmap().size();
    auto const files = dialog->selectedFiles();
    for (const QString& fName : files) {
      QImageReader newImg(fName);
      if (imgSize == newImg.size()) {
        _subimagesModel->insertRow(_subimagesModel->rowCount());
        // TODO: Internalize file
        _subimagesModel->SetData(FieldPath::Of<Sprite>(FieldPath::StartingAt(_subimagesModel->rowCount() - 1),
                                                       Sprite::kSubimagesFieldNumber),
                                 fName);
      } else {
        LoadedMismatchedImage(imgSize, newImg.size());
      }
    }
  } else {
    qDebug() << " Failed to load image: " << dialog->selectedFiles().at(0);
  }
}

void SpriteEditor::on_actionZoom_triggered() { _ui->scrollAreaWidget->ResetZoom(); }

void SpriteEditor::on_actionZoomIn_triggered() { _ui->scrollAreaWidget->ZoomIn(); }

void SpriteEditor::on_actionZoomOut_triggered() { _ui->scrollAreaWidget->ZoomOut(); }

void SpriteEditor::on_actionEditSubimages_triggered() {
  auto const selected = _ui->subImageList->selectionModel()->selectedIndexes();
  for (const QModelIndex& idx : selected) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(
        _subimagesModel->DataAtRow(idx.row()).toString()));
    // TODO: file watcher reload
    // TODO: editor settings
  }
}

void SpriteEditor::on_centerOriginButton_clicked() {
  QSize sz = _ui->subimagePreview->GetPixmap().size();
  _ui->originXSpinBox->setValue(sz.width() / 2);
  _ui->originYSpinBox->setValue(sz.height() / 2);
}
