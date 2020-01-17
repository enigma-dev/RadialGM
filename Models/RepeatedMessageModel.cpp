#include "RepeatedMessageModel.h"
#include "MessageModel.h"

RepeatedMessageModel::RepeatedMessageModel(ProtoModel *parent, Message *message, const FieldDescriptor *field)
    : RepeatedModel<Message>(parent, message, field,
                             message->GetReflection()->GetMutableRepeatedFieldRef<Message>(message, field)) {
  const Reflection *refl = _protobuf->GetReflection();
  for (int j = 0; j < refl->FieldSize(*_protobuf, field); j++) {
    _subModels.append(new MessageModel(parent, refl->MutableRepeatedMessage(_protobuf, field, j)));
  }
}

void RepeatedMessageModel::Swap(int left, int right) { std::swap(_subModels[left], _subModels[right]); }

void RepeatedMessageModel::AppendNew() {
  MessageModel *parent = GetParentModel<MessageModel *>();
  Message *parentBuffer = parent->GetBuffer();
  Message *m = parentBuffer->GetReflection()->AddMessage(parentBuffer, _field);
  _subModels.append(new MessageModel(parent, m));
}

void RepeatedMessageModel::Resize(int newSize) { _subModels.resize(newSize); }

void RepeatedMessageModel::Clear() {
  _fieldRef.Clear();
  _subModels.clear();
}

QVariant RepeatedMessageModel::Data(int row, int column) const { return _subModels[row]->Data(column); }

bool RepeatedMessageModel::SetData(const QVariant &value, int row, int column) {
  return _subModels[row]->SetData(value, column);
}

int RepeatedMessageModel::columnCount(const QModelIndex & /*parent*/) const {
  return _field->message_type()->field_count();
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
    Message *m = parentBuffer->GetReflection()->AddMessage(parentBuffer, _field);
    _subModels.append(new MessageModel(_parentModel, m));
  }

  SwapBack(row, p, rowCount());
  ParentDataChanged();

  return true;
};
