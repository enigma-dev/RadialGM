#include "RepeatedStringModel.h"
#include "Components/Logger.h"
#include "ProtoModel.h"

#include <QDataStream>
#include <QImageReader>
#include <QMimeData>

RepeatedStringModel::RepeatedStringModel(MutableRepeatedFieldRef<std::string> protobuf, const FieldDescriptor* field,
                                         ProtoModelPtr parent)
    : QAbstractListModel(parent), parentModel(parent), field(field), strings(protobuf) {
  mimeTypeStr = "RadialGM/" + QString::fromStdString(field->name());
}

int RepeatedStringModel::rowCount(const QModelIndex& /*parent*/) const { return strings.size(); }

QVariant RepeatedStringModel::data(int row) const { return data(index(row), Qt::UserRole); }

QVariant RepeatedStringModel::data(const QModelIndex& index, int role) const {
  R_EXPECT(index.isValid(), QVariant()) << "Supplied index was invalid:" << index;

  if (role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();
  return QString::fromStdString(strings.Get(index.row()));
}

bool RepeatedStringModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  R_EXPECT(index.isValid(), false) << "Supplied index was invalid:" << index;
  const QVariant oldValue = this->data(index, role);
  ParentDataChanged();
  emit dataChanged(index, index, oldValue);
  strings.Set(index.row(), value.toString().toStdString());
  return true;
}

bool RepeatedStringModel::setData(int row, const QVariant& value) { return setData(index(row, 0), value); }

bool RepeatedStringModel::insertRows(int position, int rows, const QModelIndex& /*parent*/) {
  beginInsertRows(QModelIndex(), position, position + rows - 1);

  QList<std::string> strs;
  strs.reserve(strings.size());

  for (int i = 0; i < strings.size(); ++i) {
    strs.push_back(strings.Get(i));
  }

  strings.Clear();

  for (int i = 0; i < position; ++i) {
    strings.Add(strs[i]);
  }

  for (int i = 0; i < rows; ++i) {
    strings.Add("");
  }

  for (int i = position; i < strs.size(); ++i) {
    strings.Add(strs[i]);
  }

  endInsertRows();

  GetParentModel()->SetDirty(true);

  return true;
}

bool RepeatedStringModel::removeRows(int row, int count, const QModelIndex& parent) {
  if (parent.isValid()) return false;
  if (row + count <= 0) return false;

  {
    RowRemovalOperation remover(this);
    remover.RemoveRows(row, count);
  }

  return true;
}

Qt::DropActions RepeatedStringModel::supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

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

  mimeData->setData(mimeTypeStr, encodedData);

  return mimeData;
}

bool RepeatedStringModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                                       const QModelIndex& parent) {
  if (action == Qt::IgnoreAction) return true;
  if (!data->hasFormat(mimeTypeStr)) return false;
  if (column > 0) return false;

  int beginRow;
  if (row != -1)
    beginRow = row;
  else if (parent.isValid())
    beginRow = parent.row();
  else
    beginRow = rowCount(QModelIndex());

  QByteArray encodedData = data->data(mimeTypeStr);
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

QStringList RepeatedStringModel::mimeTypes() const { return QStringList(mimeTypeStr); }

Qt::ItemFlags RepeatedStringModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

  if (index.isValid())
    return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
  else
    return Qt::ItemIsDropEnabled | defaultFlags;
}

ProtoModelPtr RepeatedStringModel::GetParentModel() const { return parentModel; }

void RepeatedStringModel::ParentDataChanged() {
  ProtoModelPtr m = GetParentModel();
  while (m != nullptr) {
    emit m->dataChanged(QModelIndex(), QModelIndex());
    m = m->GetParentModel();
  }
}

void RepeatedStringModel::clear() {
  emit beginResetModel();
  strings.Clear();
  emit endResetModel();
}

void RepeatedStringModel::SwapBack(int left, int part, int right) {
  if (left >= part || part >= right) return;
  int npart = (part - left) % (right - part);
  while (part > left) {
    strings.SwapElements(part, right);
  }
  SwapBack(left, left + npart, right);
}

void RepeatedStringModel::RowRemovalOperation::RemoveRow(int row) { rows.insert(row); }

void RepeatedStringModel::RowRemovalOperation::RemoveRows(int row, int count) {
  for (int i = row; i < row + count; ++i) rows.insert(i);
}

RepeatedStringModel::RowRemovalOperation::~RowRemovalOperation() {
  if (rows.empty()) return;

  // Compute ranges for our deleted rows.
  struct Range {
    int first, last;
    Range() : first(), last() {}
    Range(int f, int l) : first(f), last(l) {}
    int size() { return last - first + 1; }
  };
  std::vector<Range> ranges;
  for (int row : rows) {
    if (ranges.empty() || row != ranges.back().last + 1) {
      ranges.emplace_back(row, row);
    } else {
      ranges.back().last = row;
    }
  }

  // Broadcast range removal before the model can fuck anything up.
  // Do this from back to front to minimize the amount of shit it fucks up.
  model->beginResetModel();

  // Basic dense range removal. Move "deleted" rows to the end of the array.
  int left = 0, right = 0;
  for (auto range : ranges) {
    while (right < range.first) {
      field.SwapElements(left, right);
      left++;
      right++;
    }
    right = range.last + 1;
  }
  while (right < field.size()) {
    field.SwapElements(left, right);
    left++;
    right++;
  }

  // Send the endRemoveRows operations in the reverse order, removing the
  // correct number of rows incrementally, or else various components in Qt
  // will bitch, piss, moan, wail, whine, and cry. Actually, they will anyway.
  for (Range range : ranges) {
    for (int j = range.first; j <= range.last; ++j) field.RemoveLast();
    model->endResetModel();
  }

  model->GetParentModel()->SetDirty(true);
}
