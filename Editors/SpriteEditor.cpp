#include "SpriteEditor.h"
#include "Components/Utility.h"
#include "Models/MessageModel.h"

#include "ui_SpriteEditor.h"

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

  resMapper->addMapping(_ui->originXSpinBox, Sprite::kOriginXFieldNumber);
  resMapper->addMapping(_ui->originYSpinBox, Sprite::kOriginYFieldNumber);
  resMapper->addMapping(_ui->collisionShapeGroupBox, Sprite::kShapeFieldNumber, "currentIndex");
  resMapper->addMapping(_ui->bboxComboBox, Sprite::kBboxModeFieldNumber, "currentIndex");
  resMapper->addMapping(_ui->leftSpinBox, Sprite::kBboxLeftFieldNumber);
  resMapper->addMapping(_ui->rightSpinBox, Sprite::kBboxRightFieldNumber);
  resMapper->addMapping(_ui->topSpinBox, Sprite::kBboxTopFieldNumber);
  resMapper->addMapping(_ui->bottomSpinBox, Sprite::kBboxBottomFieldNumber);

  RebindSubModels();
}

SpriteEditor::~SpriteEditor() { delete _ui; }

void SpriteEditor::RebindSubModels() {
  MessageModel* _spriteModel = _model->GetSubModel<MessageModel*>(TreeNode::kSpriteFieldNumber);
  _subimagesModel = _spriteModel->GetSubModel<RepeatedImageModel*>(Sprite::kSubimagesFieldNumber);

  _ui->subImageList->setModel(_subimagesModel);
  _ui->subImageList->setIconSize(_subimagesModel->GetIconSize());
  _ui->subImageList->setGridSize(_subimagesModel->GetIconSize());

  _ui->subimagePreview->SetResourceModel(_spriteModel);
  connect(_subimagesModel, &RepeatedImageModel::MismatchedImageSize, this, &SpriteEditor::LoadedMismatchedImage);

  connect(_ui->subImageList->selectionModel(), &QItemSelectionModel::selectionChanged, this,
          &SpriteEditor::SelectionChanged);

  BaseEditor::RebindSubModels();

  on_bboxComboBox_currentIndexChanged(_spriteModel->Data(Sprite::kBboxModeFieldNumber).toInt());
}

void SpriteEditor::LoadedMismatchedImage(QSize expectedSize, QSize actualSize) {
  QString expected =
      tr("Expected: ") + QString::number(expectedSize.width()) + " x " + QString::number(expectedSize.height());

  QString actual = tr("Actual: ") + QString::number(actualSize.width()) + " x " + QString::number(actualSize.height());

  QMessageBox::critical(this, tr("Failed to load image"),
                        QString(tr("Error mismatched image sizes\n%1\n%2")).arg(expected).arg(actual), QMessageBox::Ok);
  // TODO: Add some remedies to this such as streching, croping or scaling the background
}

void SpriteEditor::RemoveSelectedIndexes() {
  /*{
    RepeatedStringModel::RowRemovalOperation remover(_subimagesModel);
    for (QModelIndex idx : ui->subImageList->selectionModel()->selectedIndexes()) {
      remover.RemoveRow(idx.row());
    }
  }*/

  _ui->subimagePreview->SetSubimage((_subimagesModel->rowCount() == 0) ? -1 : 0);
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
  _subimagesModel->SetData(fName, _subimagesModel->rowCount() - 1);
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
    QImageReader img(dialog->selectedFiles()[0]);
    if (img.size().width() > 0 && img.size().height() > 0) {
      _subimagesModel->Clear();
      for (QString fName : dialog->selectedFiles()) {
        QImageReader newImg(fName);
        if (img.size() == newImg.size()) {
          _subimagesModel->insertRow(_subimagesModel->rowCount());
          // TODO: Internalize file
          _subimagesModel->SetData(fName, _subimagesModel->rowCount() - 1);
        } else {
          LoadedMismatchedImage(img.size(), newImg.size());
        }
      }
    } else {
      qDebug() << " Failed to load image: " << dialog->selectedFiles()[0];
    }
  }
}

void SpriteEditor::on_actionAddSubimages_triggered() {
  FileDialog* dialog = new FileDialog(this, FileDialog_t::SpriteLoad, false);
  dialog->setFileMode(QFileDialog::ExistingFiles);

  if (dialog->exec() && dialog->selectedFiles().size() > 0) {
    QSize imgSize = _ui->subimagePreview->GetPixmap().size();
    for (QString fName : dialog->selectedFiles()) {
      QImageReader newImg(fName);
      if (imgSize == newImg.size()) {
        _subimagesModel->insertRow(_subimagesModel->rowCount());
        // TODO: Internalize file
        _subimagesModel->SetData(fName, _subimagesModel->rowCount() - 1);
      } else {
        LoadedMismatchedImage(imgSize, newImg.size());
      }
    }
  } else {
    qDebug() << " Failed to load image: " << dialog->selectedFiles()[0];
  }
}

void SpriteEditor::on_actionZoom_triggered() { _ui->scrollAreaWidget->ResetZoom(); }

void SpriteEditor::on_actionZoomIn_triggered() { _ui->scrollAreaWidget->ZoomIn(); }

void SpriteEditor::on_actionZoomOut_triggered() { _ui->scrollAreaWidget->ZoomOut(); }

void SpriteEditor::on_actionEditSubimages_triggered() {
  for (QModelIndex idx : _ui->subImageList->selectionModel()->selectedIndexes()) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(_subimagesModel->Data(idx.row()).toString()));
    // TODO: file watcher reload
    // TODO: editor settings
  }
}

void SpriteEditor::on_centerOriginButton_clicked() {
  QSize sz = _ui->subimagePreview->GetPixmap().size();
  _ui->originXSpinBox->setValue(sz.width() / 2);
  _ui->originYSpinBox->setValue(sz.height() / 2);
}
