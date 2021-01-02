#include "Components/Logger.h"
#include "RepeatedMessageModel.h"
#include "MessageModel.h"

RepeatedMessageModel::RepeatedMessageModel(ProtoModel *parent, Message *message, const FieldDescriptor *field)
    : BasicRepeatedModel<Message>(parent, message, field,
                             message->GetReflection()->GetMutableRepeatedFieldRef<Message>(message, field)) {
  const Reflection *refl = _protobuf->GetReflection();
  for (int j = 0; j < refl->FieldSize(*_protobuf, field); j++) {
    _subModels.append(new MessageModel(parent, refl->MutableRepeatedMessage(_protobuf, field, j)));
  }
}

void RepeatedMessageModel::SwapWithoutSignal(int left, int right) {
  BasicRepeatedModel<Message>::SwapWithoutSignal(left, right);
  std::swap(_subModels[left], _subModels[right]);
}

void RepeatedMessageModel::AppendNewWithoutSignal() {
  _subModels.append(new MessageModel(GetParentModel<MessageModel>(), field_ref_.NewMessage()));
}

void RepeatedMessageModel::RemoveLastNRowsWithoutSignal(int n) {
  BasicRepeatedModel<Message>::RemoveLastNRowsWithoutSignal(n);
  _subModels.resize(_subModels.size() - n);
}

void RepeatedMessageModel::ClearWithoutSignal() {
  field_ref_.Clear();
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

QModelIndex RepeatedMessageModel::insert(const Message &message, int row) {
  // TODO: write me
  return index(row, 0);
}

QModelIndex RepeatedMessageModel::duplicate(const QModelIndex &message) {
  // TODO: write me
  return message;
}

bool RepeatedMessageModel::SetData(const FieldPath &field_path, const QVariant &value) {
  Q_UNUSED(value);
  if (field_path.fields.empty()) {
    qDebug() << "Unimplemented: assigning a QVariant to a repeated message field.";
    return false;
  }
  qDebug() << "Attempting to set a sub-field of repeated field `" << field_path.fields[0]->full_name().c_str() << "`";
  return false;
}

QVariant RepeatedMessageModel::Data(const FieldPath &field_path) const {
  QVector<QVariant> vec;
  if (field_path.fields.empty()) {
    for (int i = 0; i < rowCount(); ++i) vec.push_back(GetDirect(i));
  } else {
    FieldPath sub_field = field_path.SubPath(1);
    for (const auto *sub_model : _subModels) vec.push_back(sub_model->Data(sub_field));
  }
  return QVariant::fromValue(vec);
}

QVariant RepeatedMessageModel::data(const QModelIndex &index, int role) const {
  R_EXPECT(index.row() >= 0 && index.row() < _subModels.size(), QVariant()) <<
    "Supplied row was out of bounds:" << index.row();

  // protobuf field number with 0 is impossible, use as sentinel to get model itself
  if (index.column() == 0)
    return QVariant::fromValue(_subModels[index.row()]);

  return _subModels[index.row()]->data(_subModels[index.row()]->index(index.column()), role);
}

Qt::ItemFlags RepeatedMessageModel::flags(const QModelIndex &index) const {
  R_EXPECT(index.row() >= 0 && index.row() < _subModels.size(), RepeatedModel::flags(index)) <<
    "Supplied row was out of bounds:" << index.row();

  return _subModels[index.row()]->flags(_subModels[index.row()]->index(index.column()));
}

const std::string &RepeatedMessageModel::MessageName() const {
  auto *msg = _field->message_type();
  if (!msg) {
    static const std::string kSentinel;
    qDebug() << "Message type of RepeatedMessageField is null! This should never happen!";
    return kSentinel;
  }
  return msg->full_name();
}
