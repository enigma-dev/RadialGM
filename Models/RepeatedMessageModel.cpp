#include "Components/Logger.h"
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

QVariant RepeatedMessageModel::Data(int row, int column) const {
  R_EXPECT(row >= 0 && row < _subModels.size(), QVariant()) <<
    "Supplied row was out of bounds:" << row;
  return _subModels[row]->Data(column);
}

bool RepeatedMessageModel::SetData(const QVariant &value, int row, int column) {
  R_EXPECT(row >= 0 && row < _subModels.size(), false) <<
    "Supplied row was out of bounds:" << row;
  return _subModels[row]->SetData(value, column);
}

int RepeatedMessageModel::columnCount(const QModelIndex & /*parent*/) const {
  return _field->message_type()->field_count();
}

bool RepeatedMessageModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  R_EXPECT(index.row() >= 0 && index.row() < _subModels.size(), false) <<
    "Supplied row was out of bounds:" << index.row();
  return _subModels[index.row()]->setData(_subModels[index.row()]->index(index.column()), value, role);
}

QVariant RepeatedMessageModel::data(const QModelIndex &index, int role) const {
  R_EXPECT(index.row() >= 0 && index.row() < _subModels.size(), QVariant()) <<
    "Supplied row was out of bounds:" << index.row();
  // protobuf field number with 0 is impossible, use as sentinel to get model itself
  if (index.column() == 0) {
    R_EXPECT(index.isValid(), QVariant()) << "Supplied index was invalid:" << index;
    return QVariant::fromValue(_subModels[index.row()]);
  }
  return _subModels[index.row()]->data(_subModels[index.row()]->index(index.column()), role);
}
