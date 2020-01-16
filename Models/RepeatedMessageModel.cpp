#include "RepeatedMessageModel.h"
#include "MessageModel.h"

QVariant RepeatedMessageModel::Data(int row, int column) const { return _subModels[row]->Data(column); }

bool RepeatedMessageModel::SetData(const QVariant &value, int row, int column) {
  return _subModels[row]->SetData(value, column);
}

bool RepeatedMessageModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  return _subModels[index.row()]->setData(_subModels[index.row()]->index(index.column()), value, role);
}

QVariant RepeatedMessageModel::data(const QModelIndex &index, int role) const {
  return _subModels[index.row()]->data(_subModels[index.row()]->index(index.column()), role);
}

QVariant RepeatedMessageModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (section == 0 || role != Qt::DisplayRole || orientation != Qt::Orientation::Horizontal) return QVariant();
  return QString::fromStdString(_field->message_type()->field(section - 1)->name());
}

QModelIndex RepeatedMessageModel::index(int row, int column, const QModelIndex & /*parent*/) const {
  return createIndex(row, column);
}

bool RepeatedMessageModel::insertRows(int row, int count, const QModelIndex &parent) {
  if (row > rowCount()) return false;

  beginInsertRows(parent, row, row + count - 1);

  int p = rowCount();

  Message *parentBuffer = GetParentModel<MessageModel *>()->GetBuffer();
  for (int r = 0; r < count; ++r) {
    //Message *m = parentBuffer->GetReflection()->AddMessage(parentBuffer, _field);
    //_subModels.append(new MessageModel(m, _parentModel));
  }

  SwapBack(row, p, rowCount());
  ParentDataChanged();

  return true;
};
