#include "BackgroundRenderer.h"

#include "Components/ArtManager.h"

#include "codegen/Background.pb.h"

#include <QMessageBox>
#include <QPainter>

using buffers::resources::Background;

BackgroundRenderer::BackgroundRenderer(QWidget *parent) : QWidget(parent), model(nullptr), zoom(1) {}

void BackgroundRenderer::SetResourceModel(ProtoModel *model) {
  this->model = model;
  SetImage(model->data(Background::kImageFieldNumber).toString());
}

bool BackgroundRenderer::SetImage(QPixmap image) {
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

bool BackgroundRenderer::SetImage(QString fName) {
  if (!SetImage(QPixmap(fName))) {
    QMessageBox::critical(this, tr("Failed to load image"), tr("Error opening: ") + fName, QMessageBox::Ok);
    return false;
  }

  return true;
}

void BackgroundRenderer::WriteImage(QString fName, QString type) {
  if (!pixmap.save(fName, type.toStdString().c_str()))
    QMessageBox::critical(this, tr("Failed to save image"), tr("Error writing: ") + fName, QMessageBox::Ok);
}

QSize BackgroundRenderer::sizeHint() const { return QSize(pixmap.width(), pixmap.height()); }

void BackgroundRenderer::SetZoom(qreal zoom) {
  if (zoom > 3200) zoom = 3200;
  if (zoom < 0.0625) zoom = 0.0625;
  this->zoom = zoom;
  setFixedSize(static_cast<int>(pixmap.width() * zoom) + 1, static_cast<int>(pixmap.height() * zoom) + 1);
}

const qreal &BackgroundRenderer::GetZoom() const { return zoom; }

void BackgroundRenderer::paintEvent(QPaintEvent * /* event */) {
  if (!model) return;

  QPainter painter(this);

  painter.fillRect(QRectF(0, 0, pixmap.width() * zoom, pixmap.height() * zoom), ArtManager::GetTransparenyBrush());

  painter.scale(zoom, zoom);
  bool transparent = model->data(Background::kTransparentFieldNumber).toBool();
  painter.drawPixmap(0, 0, (transparent) ? transparentPixmap : pixmap);

  bool gridVisible = model->data(Background::kUseAsTilesetFieldNumber).toBool();
  int gridHorSpacing = model->data(Background::kHorizontalSpacingFieldNumber).toInt();
  int gridVertSpacing = model->data(Background::kVerticalSpacingFieldNumber).toInt();
  int gridHorOff = model->data(Background::kHorizontalOffsetFieldNumber).toInt();
  int gridVertOff = model->data(Background::kVerticalOffsetFieldNumber).toInt();
  int gridWidth = model->data(Background::kTileWidthFieldNumber).toInt();
  int gridHeight = model->data(Background::kTileHeightFieldNumber).toInt();

  if (gridVisible) {
    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    painter.setPen(QColor(0xff, 0xff, 0xff));

    if (gridHorSpacing != 0 || gridVertSpacing != 0) {
      for (int x = gridHorOff; x < pixmap.width(); x += gridWidth + gridHorSpacing) {
        for (int y = gridVertOff; y < pixmap.height(); y += gridHeight + gridVertSpacing) {
          painter.drawRect(x, y, gridWidth, gridHeight);
        }
      }
    }

    if (gridHorSpacing == 0) {
      for (int x = gridHorOff; x <= pixmap.width(); x += gridWidth) painter.drawLine(x, 0, x, pixmap.height());
    }

    if (gridVertSpacing == 0) {
      for (int y = gridVertOff; y <= pixmap.width(); y += gridHeight) painter.drawLine(0, y, pixmap.width(), y);
    }
  }
}
