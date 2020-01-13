#include "Utility.h"

#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QMap>
#include <QMimeDatabase>

struct MimeType {
  MimeType() {}
  MimeType(QString filter, QString desc) : fileFilter(filter), description(desc) {}
  QString fileFilter;
  QString description;
};

class MimeTypeList {
 public:
  MimeTypeList() {}
  MimeTypeList(const QList<QByteArray>& mimeTypes, const QList<MimeType> additionalMimeTypes) {
    QStringList temp;
    foreach (const QByteArray& mime, mimeTypes)
      temp.append(mime);

    QMimeDatabase mimeDB;
    for (const QString& mimeTypeFilter : temp) {
      QMimeType mimeType = mimeDB.mimeTypeForName(mimeTypeFilter);
      if (mimeType.isValid()) {
        nameFilters.append(mimeType.name() + " " + QString("(%1)").arg(mimeType.globPatterns().join(' ')));
        filters.append(QString("%1").arg(mimeType.globPatterns().join(' ')));
      }
    }

    for (const MimeType& mime : additionalMimeTypes) {
      filters.append(mime.fileFilter);
    }

    for (const MimeType& mime : additionalMimeTypes) {
      nameFilters.append(mime.description);
    }

    filters.prepend(QString("(%1)").arg(filters.join(' ')));
    nameFilters.prepend("All Supported " + filters[0]);
    nameFilters.append("All Files (*)");
  }

  QStringList nameFilters;
  QStringList filters;
  QString allSupported;
};

static const QMap<FileDialog_t, MimeTypeList> derp = {

    {FileDialog_t::SpriteLoad,
     MimeTypeList(QImageReader::supportedMimeTypes(), {MimeType("*.Sprite.gmx", "GMX Background (*.Background.gmx)"),
                                                       MimeType("*.spr", "EGM Background (*.bkg)")})},

    {FileDialog_t::SpriteSave,
     MimeTypeList(QImageWriter::supportedMimeTypes(), {MimeType("*.spr", "EGM Background (*.bkg)")})},

    {FileDialog_t::BackgroundLoad, MimeTypeList(QImageReader::supportedMimeTypes(),
                                                {MimeType("*.Background.gmx", "GMX Background (*.Background.gmx)"),
                                                 MimeType("*.bkg", "EGM Background (*.bkg)")})},

    {FileDialog_t::BackgroundSave,
     MimeTypeList(QImageWriter::supportedMimeTypes(), {MimeType("*.bkg", "EGM Background (*.bkg)")})},

    {FileDialog_t::SoundLoad, MimeTypeList({},
                                           {
                                               MimeType("*.Sound.gmx", "GMX Sound (*.Sound.gmx)"),
                                               MimeType("*.snd", "EGM Sound (*.snd)"),
                                               MimeType("*.ogg", "OGG (*.ogg)"),
                                               MimeType("*.flac", "FLAC (*.flac)"),
                                               MimeType("*.mp3", "MP3 (*.mp3)"),
                                               MimeType("*.mod", "MOD (*.mod)"),
                                               MimeType("*.wav", "Wav (*.wav)"),
                                           })},

    {FileDialog_t::SoundSave, MimeTypeList({}, {MimeType("*.snd", "EGM Sound (*.snd)")})}};

FileDialog::FileDialog(QWidget* parent, FileDialog_t type, bool writer) : QFileDialog(parent, "Select ") {
  if (writer)
    setAcceptMode(QFileDialog::AcceptSave);
  else {
    setAcceptMode(QFileDialog::AcceptOpen);
    setFileMode(QFileDialog::ExistingFile);
  }

  const MimeTypeList& mimes = derp[type];
  setMimeTypeFilters(mimes.filters);
  setNameFilters(mimes.nameFilters);
  selectNameFilter(mimes.allSupported);
}
