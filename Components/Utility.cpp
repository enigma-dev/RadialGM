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
