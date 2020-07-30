#include "BackgroundView.h"

#include "Components/ArtManager.h"

#include "Background.pb.h"

#include <QMessageBox>
#include <QPainter>

using buffers::resources::Background;

BackgroundView::BackgroundView(AssetScrollAreaBackground *parent) : AssetView(parent) {
  _grid.type = GridType::Complex;
  parent->SetDrawSolidBackground(true, Qt::GlobalColor::transparent);
}

bool BackgroundView::SetImage(QPixmap image) {
  if (image.isNull()) return false;

  _pixmap = image;

  QImage img = _pixmap.toImage();
  img = img.convertToFormat(QImage::Format_ARGB32);
  _transparencyColor = img.pixelColor(0, img.height() - 1);
  for (int x = 0; x < img.width(); ++x) {
    for (int y = 0; y < img.height(); ++y) {
      if (img.pixelColor(x, y) == _transparencyColor) img.setPixelColor(x, y, Qt::transparent);
    }
  }

  _transparentPixmap = QPixmap::fromImage(img);
  setFixedSize(_pixmap.width() + 1, _pixmap.height() + 1);
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
  if (!_pixmap.save(fName, type.toStdString().c_str()))
    QMessageBox::critical(this, tr("Failed to save image"), tr("Error writing: ") + fName, QMessageBox::Ok);
}

QSize BackgroundView::sizeHint() const { return QSize(_pixmap.width(), _pixmap.height()); }

void BackgroundView::Paint(QPainter &painter) {

}
