#include "RepeatedModel.h"

#include <QDataStream>
#include <QImageReader>
#include <QMimeData>

#include <algorithm>

bool RepeatedModel::setData(const QModelIndex& index, const QVariant& value, int /*role*/) {
  if (index.column() != 0) {
    qDebug() << "Invalid column index";
    return false;
  }

  if (index.row() < 0 || index.row() >= rowCount()) {
    qDebug() << "Invalid row index";
    return false;
  }

  return SetDirect(index.row(), value);
}

QVariant RepeatedModel::data(const QModelIndex& index, int /*role*/) const {
  if (index.column() != 0) {
    qDebug() << "Invalid column index";
    return QVariant();
  }

  if (index.row() < 0 || index.row() >= rowCount()) {
    qDebug() << "Invalid row index";
    return QVariant();
  }

  return GetDirect(index.row());
}

bool RepeatedModel::SetData(const FieldPath &field_path, const QVariant &value) {
  Q_UNUSED(value);
  if (!field_path.fields.empty()) {
    qDebug() << "Attempting to set a sub-field of repeated field `" << field_path.fields[0]->full_name().c_str() << "`";
    return false;
  }
  if (field_path.repeated_field_index != -1) {
    if (field_path.repeated_field_index < rowCount()) {
      return SetDirect(field_path.repeated_field_index, value);
    }
    // XXX: Allow append when *just* out of bounds?
    qDebug() << "Attempting to assign out-of-bounds index " << field_path.repeated_field_index << " of field `"
             << field_path.fields[0]->full_name().c_str() << "`";
    return false;
  }
  qDebug() << "Unimplemented: assigning a QVariant to a repeated field.";
  return false;
}

QVariant RepeatedModel::Data(const FieldPath &field_path) const {
  if (!field_path.fields.empty()) {
    // FieldPath sub = field_path.SubPath(1);
    // for (int i = 0; i < rowCount(); ++i) vec.push_back(Data(sub));
    qDebug() << "Attempting to access a sub-field of a repeated field...";
    return QVariant();
  }
  if (field_path.repeated_field_index) {
    if (field_path.repeated_field_index < rowCount()) {
      return GetDirect(field_path.repeated_field_index);
    }
    qDebug() << "Attempting to retrieve out-of-bounds index " << field_path.repeated_field_index << " of field `"
             << field_path.fields[0]->full_name().c_str() << "`";
    return QVariant();
  }
  QVector<QVariant> vec;
  for (int i = 0; i < rowCount(); ++i) vec.push_back(GetDirect(i));
  return QVariant::fromValue(vec);
}

bool RepeatedModel::moveRows(const QModelIndex &sourceParent, int source, int count,
                             const QModelIndex &destinationParent, int destination) {
  int left = source;
  int right = source + count;
  if (left < 0 || destination < 0) return false;
  if (right > rowCount() || destination > rowCount()) return false;
  if (destination >= left && destination < right) return false;

  beginMoveRows(sourceParent, source, source + count - 1, destinationParent, destination);

  if (destination < left) {
    SwapBackWithoutSignal(destination, left, right);
  } else {  // Verified above that we're dest < left or dest >= right
    SwapBackWithoutSignal(left, right, destination);
  }

  endMoveRows();
  ParentDataChanged();

  return true;
}

QMimeData* RepeatedModel::mimeData(const QModelIndexList& indexes) const {
  QModelIndexList sortedIndexes = indexes;
  std::sort(sortedIndexes.begin(), sortedIndexes.end(),
            [](QModelIndex& a, QModelIndex& b) { return a.row() < b.row(); });

  QMimeData* mimeData = new QMimeData();
  QByteArray encodedData;
  QDataStream stream(&encodedData, QIODevice::WriteOnly);

  foreach (const QModelIndex& index, sortedIndexes) {
    if (index.isValid()) {
      QString text = data(index, Qt::UserRole).toString();
      stream << text;
      stream << index.row();
    }
  }

  mimeData->setData(mimeTypes()[0], encodedData);

  return mimeData;
}

bool RepeatedModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                                 const QModelIndex& parent) {
  if (row < 0) return false;
  if (action == Qt::IgnoreAction) return true;
  if (!data->hasFormat(mimeTypes()[0])) return false;
  if (column > 0) return false;

  int beginRow;
  if (row != -1)
    beginRow = row;
  else if (parent.isValid())
    beginRow = parent.row();
  else
    beginRow = rowCount(QModelIndex());

  QByteArray encodedData = data->data(mimeTypes()[0]);
  QDataStream stream(&encodedData, QIODevice::ReadOnly);
  QStringList newItems;
  int rows = 0;

  while (!stream.atEnd()) {
    QString text;
    stream >> text;
    int index;
    stream >> index;
    newItems << text;
    ++rows;
  }

  insertRows(beginRow, rows, QModelIndex());
  foreach (const QString& text, newItems) {
    QModelIndex idx = index(beginRow, 0, QModelIndex());
    setData(idx, text, Qt::UserRole);
    beginRow++;
  }

  return true;
}
