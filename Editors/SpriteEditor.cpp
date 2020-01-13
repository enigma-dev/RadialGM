#include "SpriteEditor.h"
#include "Components/Utility.h"

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

SpriteEditor::SpriteEditor(ProtoModelPtr model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::SpriteEditor) {
  ui->setupUi(this);
  connect(ui->actionSave, &QAction::triggered, this, &BaseEditor::OnSave);
  ui->scrollAreaWidget->SetAssetView(ui->subimagePreview);

  QLabel* bboxLabel = new QLabel(tr("Show BBox "));
  QCheckBox* showBBox = new QCheckBox(this);
  showBBox->setChecked(true);
  ui->mainToolBar->addWidget(bboxLabel);
  ui->mainToolBar->addWidget(showBBox);
  connect(showBBox, &QCheckBox::stateChanged, ui->subimagePreview, &SpriteView::SetShowBBox);

  QLabel* originLabel = new QLabel(tr("Show Origin "));
  QCheckBox* showOrigin = new QCheckBox(this);
  showOrigin->setChecked(true);
  ui->mainToolBar->addWidget(originLabel);
  ui->mainToolBar->addWidget(showOrigin);
  connect(showOrigin, &QCheckBox::stateChanged, ui->subimagePreview, &SpriteView::SetShowOrigin);

  resMapper->addMapping(ui->originXSpinBox, Sprite::kOriginXFieldNumber);
  resMapper->addMapping(ui->originYSpinBox, Sprite::kOriginYFieldNumber);
  resMapper->addMapping(ui->collisionShapeGroupBox, Sprite::kShapeFieldNumber, "currentIndex");
  resMapper->addMapping(ui->bboxComboBox, Sprite::kBboxModeFieldNumber, "currentIndex");
  resMapper->addMapping(ui->leftSpinBox, Sprite::kBboxLeftFieldNumber);
  resMapper->addMapping(ui->rightSpinBox, Sprite::kBboxRightFieldNumber);
  resMapper->addMapping(ui->topSpinBox, Sprite::kBboxTopFieldNumber);
  resMapper->addMapping(ui->bottomSpinBox, Sprite::kBboxBottomFieldNumber);

  RebindSubModels();
}

SpriteEditor::~SpriteEditor() { delete ui; }

void SpriteEditor::RebindSubModels() {
  _spriteModel = _model->GetSubModel(TreeNode::kSpriteFieldNumber);
  _subimagesModel =
      static_cast<SpriteSubimageModelPtr>(_spriteModel->GetRepeatedStringSubModel(Sprite::kSubimagesFieldNumber));

  ui->subImageList->setModel(_subimagesModel);
  ui->subImageList->setIconSize(_subimagesModel->GetIconSize());
  ui->subImageList->setGridSize(_subimagesModel->GetIconSize());

  ui->subimagePreview->SetResourceModel(_spriteModel);
  connect(_subimagesModel, &SpriteSubimageModel::MismatchedImageSize, this, &SpriteEditor::LoadedMismatchedImage);

  connect(ui->subImageList->selectionModel(), &QItemSelectionModel::selectionChanged, this,
          &SpriteEditor::SelectionChanged);

  BaseEditor::RebindSubModels();

  on_bboxComboBox_currentIndexChanged(_spriteModel->data(Sprite::kBboxModeFieldNumber).toInt());
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
  {
    RepeatedStringModel::RowRemovalOperation remover(_subimagesModel);
    for (QModelIndex idx : ui->subImageList->selectionModel()->selectedIndexes()) {
      remover.RemoveRow(idx.row());
    }
  }

  ui->subimagePreview->SetSubimage((_subimagesModel->rowCount() == 0) ? -1 : 0);
}

void SpriteEditor::SelectionChanged(const QItemSelection& selected, const QItemSelection& /*deselected*/) {
  if (!selected.empty()) {
    ui->subimagePreview->SetSubimage(selected.indexes().back().row());
  }
}

void SpriteEditor::on_bboxComboBox_currentIndexChanged(int index) {
  bool manual = (index != Sprite::BoundingBox::Sprite_BoundingBox_MANUAL);
  ui->leftSpinBox->setDisabled(manual);
  ui->rightSpinBox->setDisabled(manual);
  ui->topSpinBox->setDisabled(manual);
  ui->bottomSpinBox->setDisabled(manual);

  if (index == Sprite::BoundingBox::Sprite_BoundingBox_AUTOMATIC) {
    QRectF rect = ui->subimagePreview->AutomaticBBoxRect();
    ui->leftSpinBox->setValue(rect.x());
    ui->topSpinBox->setValue(rect.y());
    ui->rightSpinBox->setValue(rect.x() + rect.width());
    ui->bottomSpinBox->setValue(rect.y() + rect.height());
  } else if (index == Sprite::BoundingBox::Sprite_BoundingBox_FULL_IMAGE) {
    QPixmap p = ui->subimagePreview->GetPixmap();
    ui->leftSpinBox->setValue(0);
    ui->topSpinBox->setValue(0);
    ui->rightSpinBox->setValue(p.width());
    ui->bottomSpinBox->setValue(p.height());
  }
}

void SpriteEditor::on_actionNewSubimage_triggered() {
  QSize imgSize;
  if (_subimagesModel->rowCount() == 0) {
    // TODO: Dialog to ask size
    imgSize = QSize(64, 64);
  } else {
    imgSize = ui->subimagePreview->GetPixmap().size();
  }

  QImage img(imgSize, QImage::Format_RGBA8888);
  img.fill(Qt::red);
  // TODO: Generate real name and save in proper directory inside the EGM
  QString uid = QUuid::createUuid().toString();
  QString fName(QDir::tempPath() + "/" + uid.mid(1, uid.length() - 2) + ".png");
  img.save(fName);
  _subimagesModel->insertRow(_subimagesModel->rowCount());
  _subimagesModel->setData(_subimagesModel->rowCount() - 1, fName);
}

void SpriteEditor::on_actionDeleteSubimages_triggered() { RemoveSelectedIndexes(); }

void SpriteEditor::on_actionCut_triggered() {
  QGuiApplication::clipboard()->setMimeData(
      _subimagesModel->mimeData(ui->subImageList->selectionModel()->selectedIndexes()));
  RemoveSelectedIndexes();
}

void SpriteEditor::on_actionPaste_triggered() {
  _subimagesModel->dropMimeData(QGuiApplication::clipboard()->mimeData(), Qt::DropAction::CopyAction,
                                _subimagesModel->rowCount(), 0, QModelIndex());
}

void SpriteEditor::on_actionCopy_triggered() {
  QGuiApplication::clipboard()->setMimeData(
      _subimagesModel->mimeData(ui->subImageList->selectionModel()->selectedIndexes()));
}

void SpriteEditor::on_actionLoadSubimages_triggered() {
  FileDialog* dialog = new FileDialog(this, FileDialog_t::BackgroundLoad, false);
  dialog->setFileMode(QFileDialog::ExistingFiles);

  if (dialog->exec() && dialog->selectedFiles().size() > 0) {
    QImageReader img(dialog->selectedFiles()[0]);
    if (img.size().width() > 0 && img.size().height() > 0) {
      _subimagesModel->clear();
      for (QString fName : dialog->selectedFiles()) {
        QImageReader newImg(fName);
        if (img.size() == newImg.size()) {
          _subimagesModel->insertRow(_subimagesModel->rowCount());
          // TODO: Internalize file
          _subimagesModel->setData(_subimagesModel->rowCount() - 1, fName);
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
    QSize imgSize = ui->subimagePreview->GetPixmap().size();
    for (QString fName : dialog->selectedFiles()) {
      QImageReader newImg(fName);
      if (imgSize == newImg.size()) {
        _subimagesModel->insertRow(_subimagesModel->rowCount());
        // TODO: Internalize file
        _subimagesModel->setData(_subimagesModel->rowCount() - 1, fName);
      } else {
        LoadedMismatchedImage(imgSize, newImg.size());
      }
    }
  } else {
    qDebug() << " Failed to load image: " << dialog->selectedFiles()[0];
  }
}

void SpriteEditor::on_actionZoom_triggered() { ui->scrollAreaWidget->ResetZoom(); }

void SpriteEditor::on_actionZoomIn_triggered() { ui->scrollAreaWidget->ZoomIn(); }

void SpriteEditor::on_actionZoomOut_triggered() { ui->scrollAreaWidget->ZoomOut(); }

void SpriteEditor::on_actionEditSubimages_triggered() {
  for (QModelIndex idx : ui->subImageList->selectionModel()->selectedIndexes()) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(_subimagesModel->data(idx.row()).toString()));
    // TODO: file watcher reload
    // TODO: editor settings
  }
}

void SpriteEditor::on_centerOriginButton_clicked() {
  QSize sz = ui->subimagePreview->GetPixmap().size();
  ui->originXSpinBox->setValue(sz.width() / 2);
  ui->originYSpinBox->setValue(sz.height() / 2);
}
