#include "SpriteModel.h"

#include <QIcon>
#include <QImageReader>
#include <QMimeData>

#include <QDebug>
#include <QtGlobal>

SpriteModel::SpriteModel(google::protobuf::RepeatedPtrField<std::string>* protobuf, QObject* parent)
    : QAbstractListModel(parent), protobuf(protobuf), maxIconSize(128, 128), minIconSize(16, 16) {}

void SpriteModel::SetMaxIconSize(unsigned width, unsigned height) {
  maxIconSize = QSize(static_cast<int>(width), static_cast<int>(height));
}

void SpriteModel::SetMinIconSize(unsigned width, unsigned height) {
  minIconSize = QSize(static_cast<int>(width), static_cast<int>(height));
}

QSize SpriteModel::GetIconSize() { return data(index(0), Qt::SizeHintRole).toSize(); }

int SpriteModel::rowCount(const QModelIndex& /*parent*/) const { return protobuf->size(); }
QVariant SpriteModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    qWarning() << index << "provided to SpriteModel::data for role" << role << "is invalid.";
    return QVariant();
  }

  if (role == Qt::DecorationRole)
    return QIcon(QString::fromStdString(protobuf->Get(index.row())));
  else if (role == Qt::BackgroundColorRole) {
    return QVariant(QColor(Qt::yellow));
  } else if (role == Qt::SizeHintRole) {
    // Don't load image we just need size
    QImageReader img(QString::fromStdString(protobuf->Get(index.row())));
    QSize actualSize = img.size();
    float aspectRatio = static_cast<float>(qMin(actualSize.width(), actualSize.height())) /
                        qMax(actualSize.width(), actualSize.height());

    int width = qMin(actualSize.width(), maxIconSize.width());
    int height = qMin(actualSize.height(), maxIconSize.height());

    if (actualSize.width() > maxIconSize.width() || actualSize.height() > maxIconSize.height()) {
      if (actualSize.width() < actualSize.height()) width *= aspectRatio;
      if (actualSize.width() > actualSize.height()) height *= aspectRatio;
    }

    return QSize(qMax(minIconSize.width(), width), qMax(minIconSize.height(), height));

  } else if (role == Qt::UserRole) {
    return QString::fromStdString(protobuf->Get(index.row()));
  } else
    return QVariant();
}

bool SpriteModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (!index.isValid()) {
    qWarning() << index << "provided to SpriteModel::setData for role" << role << "with value" << value
               << "is invalid.";
    return false;
  }

  if (role == Qt::UserRole) {
    qDebug() << "setData()";
    qDebug() << value.toString();
    protobuf->Mutable(index.row())->assign(value.toString().toStdString());

    qDebug() << "new list:";
    for (int i = 0; i < protobuf->size(); i++) qDebug() << QString::fromStdString(protobuf->Get(i));
  }
  return true;
}

bool SpriteModel::insertRows(int position, int rows, const QModelIndex& /*parent*/) {
  beginInsertRows(QModelIndex(), position, position + rows - 1);

  QList<std::string> images;
  images.reserve(protobuf->size());

  for (int i = 0; i < protobuf->size(); ++i) {
    images.push_back(protobuf->Get(i));
  }

  protobuf->Clear();

  for (int i = 0; i < position; ++i) {
    std::string* str = protobuf->Add();
    str->assign(images[i]);
  }

  for (int i = 0; i < rows; ++i) {
    protobuf->Add();
  }

  for (int i = position; i < images.size(); ++i) {
    std::string* str = protobuf->Add();
    str->assign(images[i]);
  }

  endInsertRows();

  return true;
}

bool SpriteModel::removeRows(int row, int count, const QModelIndex& parent) {
  if (parent.isValid()) return false;
  if (row + count <= 0) return false;

  beginRemoveRows(parent, row, row + count - 1);
  protobuf->DeleteSubrange(row, count);
  endRemoveRows();

  return true;
}

Qt::DropActions SpriteModel::supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

QMimeData* SpriteModel::mimeData(const QModelIndexList& indexes) const {
  qDebug() << "mimeData()";

  QMimeData* mimeData = new QMimeData();
  QByteArray encodedData;
  QDataStream stream(&encodedData, QIODevice::WriteOnly);

  foreach (const QModelIndex& index, indexes) {
    if (index.isValid()) {
      QString text = data(index, Qt::UserRole).toString();
      stream << text;
    }
  }

  mimeData->setData("RadialGM/Sprite-SubImages", encodedData);
  mimeData->setProperty("ImageSize", data(index(0), Qt::SizeHintRole));

  return mimeData;
}

bool SpriteModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                               const QModelIndex& parent) {
  if (action == Qt::IgnoreAction) return true;
  if (!data->hasFormat("RadialGM/Sprite-SubImages")) return false;
  if (column > 0) return false;

  QSize expectedSize = this->data(index(0), Qt::SizeHintRole).toSize();
  QSize actualSize = data->property("ImageSize").toSize();
  if (expectedSize != actualSize) {
    emit MismatchedImageSize(expectedSize, actualSize);
    return false;
  }

  qDebug() << "dropMimeData()";

  int beginRow;
  if (row != -1)
    beginRow = row;
  else if (parent.isValid())
    beginRow = parent.row();
  else
    beginRow = rowCount(QModelIndex());

  QByteArray encodedData = data->data("RadialGM/Sprite-SubImages");
  QDataStream stream(&encodedData, QIODevice::ReadOnly);
  QStringList newItems;
  int rows = 0;

  while (!stream.atEnd()) {
    QString text;
    stream >> text;
    newItems << text;
    ++rows;
  }

  qDebug() << newItems;

  insertRows(beginRow, rows, QModelIndex());
  foreach (const QString& text, newItems) {
    QModelIndex idx = index(beginRow, 0, QModelIndex());
    setData(idx, text, Qt::UserRole);
    beginRow++;
  }

  return true;
}

QStringList SpriteModel::mimeTypes() const { return QStringList("RadialGM/Sprite-SubImages"); }

Qt::ItemFlags SpriteModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

  if (index.isValid())
    return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
  else
    return Qt::ItemIsDropEnabled | defaultFlags;
}
