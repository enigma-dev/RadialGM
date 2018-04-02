#include "Utility.h"

#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QMimeDatabase>

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
  QImage img = pixmap.toImage();
  img = img.convertToFormat(QImage::Format_ARGB32);
  QColor transparencyColor = img.pixelColor(0, img.height() - 1);
  for (int x = 0; x < img.width(); ++x) {
    for (int y = 0; y < img.height(); ++y) {
      if (img.pixelColor(x, y) == transparencyColor) img.setPixelColor(x, y, Qt::transparent);
    }
  }

  return QPixmap::fromImage(img);
}
