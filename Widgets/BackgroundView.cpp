#include "BackgroundView.h"

#include "Components/ArtManager.h"

#include "Background.pb.h"

#include <QMessageBox>
#include <QPainter>

using buffers::resources::Background;

BackgroundView::BackgroundView(QWidget *parent) : AssetView(parent), model(nullptr) {}

void BackgroundView::SetResourceModel(ProtoModelPtr model) {
  this->model = model;
  SetImage(model->data(Background::kImageFieldNumber).toString());
}

bool BackgroundView::SetImage(QPixmap image) {
  if (image.isNull()) return false;

  zoom = 1;
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

void BackgroundView::paintEvent(QPaintEvent * /* event */) {
  if (!model) return;

  QPainter painter(this);

  painter.fillRect(QRectF(0, 0, pixmap.width() * zoom, pixmap.height() * zoom), ArtManager::GetTransparenyBrush());

  painter.scale(zoom, zoom);
  bool transparent = false;
  painter.drawPixmap(0, 0, (transparent) ? transparentPixmap : pixmap);

  bool gridVisible = model->data(Background::kUseAsTilesetFieldNumber).toBool();
  int gridHorSpacing = model->data(Background::kHorizontalSpacingFieldNumber).toInt();
  int gridVertSpacing = model->data(Background::kVerticalSpacingFieldNumber).toInt();
  int gridHorOff = model->data(Background::kHorizontalOffsetFieldNumber).toInt();
  int gridVertOff = model->data(Background::kVerticalOffsetFieldNumber).toInt();
  int gridWidth = model->data(Background::kTileWidthFieldNumber).toInt();
  int gridHeight = model->data(Background::kTileHeightFieldNumber).toInt();

  if (gridVisible) {
    paintGrid(painter, pixmap.width(), pixmap.height(), gridHorSpacing, gridVertSpacing, gridHorOff, gridVertOff,
              gridWidth, gridHeight);
  }
}
