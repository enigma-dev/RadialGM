#include "RepeatedStringModel.h"

#include <QDataStream>
#include <QImageReader>
#include <QMimeData>

#include <algorithm>

void RepeatedStringModel::AppendNew() { _fieldRef.Add(""); }

QVariant RepeatedStringModel::Data(int row, int column) const {
  return data(index(row, column, QModelIndex()), Qt::UserRole);
}

bool RepeatedStringModel::SetData(const QVariant& value, int row, int column) {
  return setData(index(row, column, QModelIndex()), value);
}

bool RepeatedStringModel::setData(const QModelIndex& index, const QVariant& value, int /*role*/) {
  if (index.column() != 0) {
    qDebug() << "Invalid column index";
    return false;
  }

  if (index.row() < 0 || index.row() >= rowCount()) {
    qDebug() << "Invalid row index";
    return false;
  }

  _fieldRef.Set(index.row(), value.toString().toStdString());

  return true;
}

QVariant RepeatedStringModel::data(const QModelIndex& index, int /*role*/) const {
  if (index.column() != 0) {
    qDebug() << "Invalid column index";
    return QVariant();
  }

  if (index.row() < 0 || index.row() >= rowCount()) {
    qDebug() << "Invalid row index";
    return QVariant();
  }

  return QString::fromStdString(_fieldRef.Get(index.row()));
}

QMimeData* RepeatedStringModel::mimeData(const QModelIndexList& indexes) const {
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

bool RepeatedStringModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
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
