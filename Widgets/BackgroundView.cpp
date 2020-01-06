#include "BackgroundView.h"

#include "Components/ArtManager.h"

#include "Background.pb.h"

#include <QMessageBox>
#include <QPainter>

using buffers::resources::Background;

BackgroundView::BackgroundView(AssetScrollAreaBackground *parent) : AssetView(parent), model(nullptr) {
  grid.type = GridType::Complex;
  parent->SetDrawSolidBackground(true, Qt::GlobalColor::transparent);
}

void BackgroundView::SetResourceModel(ProtoModelPtr model) {
  this->model = model;
  SetImage(model->data(Background::kImageFieldNumber).toString());
}

bool BackgroundView::SetImage(QPixmap image) {
  if (image.isNull()) return false;

  pixmap = image;

  QImage img = pixmap.toImage();
  img = img.convertToFormat(QImage::Format_ARGB32);
  transparencyColor = img.pixelColor(0, img.height() - 1);
  for (int x = 0; x < img.width(); ++x) {
    for (int y = 0; y < img.height(); ++y) {
      if (img.pixelColor(x, y) == transparencyColor) img.setPixelColor(x, y, Qt::transparent);
    }
  }

  transparentPixmap = QPixmap::fromImage(img);
  setFixedSize(pixmap.width() + 1, pixmap.height() + 1);
  update();

  return true;
}

bool BackgroundView::SetImage(QString fName) {
  if (!SetImage(ArtManager::GetCachedPixmap(fName))) {
    QMessageBox::critical(this, tr("Failed to load image"), tr("Error opening: ") + fName, QMessageBox::Ok);
    return false;
  }

  return true;
}

void BackgroundView::WriteImage(QString fName, QString type) {
  if (!pixmap.save(fName, type.toStdString().c_str()))
    QMessageBox::critical(this, tr("Failed to save image"), tr("Error writing: ") + fName, QMessageBox::Ok);
}

QSize BackgroundView::sizeHint() const { return QSize(pixmap.width(), pixmap.height()); }

void BackgroundView::Paint(QPainter &painter) {
  if (!model) {
    grid.show = false;
    return;
  }

  painter.fillRect(QRectF(0, 0, pixmap.width(), pixmap.height()), ArtManager::GetTransparenyBrush());

  bool transparent = false;
  painter.drawPixmap(0, 0, (transparent) ? transparentPixmap : pixmap);

  if (model->data(Background::kUseAsTilesetFieldNumber).toBool()) {
    grid.show = true;
    grid.horSpacing = model->data(Background::kHorizontalSpacingFieldNumber).toInt();
    grid.vertSpacing = model->data(Background::kVerticalSpacingFieldNumber).toInt();
    grid.horOff = model->data(Background::kHorizontalOffsetFieldNumber).toInt();
    grid.vertOff = model->data(Background::kVerticalOffsetFieldNumber).toInt();
    grid.cellWidth = model->data(Background::kTileWidthFieldNumber).toInt();
    grid.cellHeight = model->data(Background::kTileHeightFieldNumber).toInt();
    grid.width = pixmap.width();
    grid.height = pixmap.height();
  } else {
    grid.show = false;
  }
}
