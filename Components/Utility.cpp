#include "Utility.h"

#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QMimeDatabase>
#include <QPainter>

ImageDialog::ImageDialog(QWidget* parent, QString xmlExtension, bool writer) : QFileDialog(parent, "Select image") {
  QString fileExt("*." + xmlExtension + ".gmx");
  QString xmlName = xmlExtension;
  xmlName[0] = xmlName[0].toUpper();

  QStringList mimeTypeFilters;
  const QByteArrayList supportedMimeTypes =
      (writer) ? QImageWriter::supportedMimeTypes() : QImageReader::supportedMimeTypes();
  foreach (const QByteArray& mimeTypeName, supportedMimeTypes) { mimeTypeFilters.append(mimeTypeName); }
  mimeTypeFilters.sort(Qt::CaseInsensitive);

  QMimeDatabase mimeDB;
  QStringList allSupportedFormats;
  for (const QString& mimeTypeFilter : mimeTypeFilters) {
    QMimeType mimeType = mimeDB.mimeTypeForName(mimeTypeFilter);
    if (mimeType.isValid()) {
      allSupportedFormats.append(mimeType.globPatterns());
    }
  }
  allSupportedFormats.append(fileExt);
  QString allSupportedFormatsFilter = QString("All supported formats (%1)").arg(allSupportedFormats.join(' '));

  setFileMode(QFileDialog::ExistingFile);
  setMimeTypeFilters(mimeTypeFilters);
  QStringList nameFilters = this->nameFilters();

  if (writer)
    setAcceptMode(QFileDialog::AcceptSave);
  else
    nameFilters.append("GMX " + xmlName + " (" + fileExt + ")");

  nameFilters.prepend(allSupportedFormatsFilter);
  nameFilters.prepend("All Files (*)");
  setNameFilters(nameFilters);
  selectNameFilter(allSupportedFormatsFilter);
}

QPixmap CreateTransparentImage(const QPixmap& pixmap) {
  return CreateTransparentImage(pixmap, pixmap.width(), pixmap.height());
}

QPixmap CreateTransparentImage(const QPixmap& pixmap, int width, int height) {
  QPixmap scaled(width, height);
  QImage img = scaled.toImage();
  QPainter painter(&img);
  painter.drawTiledPixmap(img.rect(), QPixmap(":/transparent.png"));
  if (pixmap.width() > pixmap.height()) {
    painter.translate(0, height / 2 - (pixmap.height() * (width / pixmap.width())) / 2);
    painter.scale(width / pixmap.width(), width / pixmap.width());

  } else {
    painter.translate(width / 2 - (pixmap.width() * (height / pixmap.height())) / 2, 0);
    painter.scale(height / pixmap.height(), height / pixmap.height());
  }

  painter.drawPixmap(pixmap.rect(), pixmap);
  painter.end();

  return QPixmap::fromImage(img);
}
