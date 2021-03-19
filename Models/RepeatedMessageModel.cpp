#include "Components/Logger.h"
#include "RepeatedMessageModel.h"
#include "MessageModel.h"

RepeatedMessageModel::RepeatedMessageModel(ProtoModel *parent, Message *message, const FieldDescriptor *field)
    : BasicRepeatedModel<Message>(parent, message, field,
                             message->GetReflection()->GetMutableRepeatedFieldRef<Message>(message, field)) {
  const Reflection *refl = _protobuf->GetReflection();
  for (int j = 0; j < refl->FieldSize(*_protobuf, field); j++) {
    _subModels.append(new MessageModel(this, refl->MutableRepeatedMessage(_protobuf, field, j), j));
  }
}

void RepeatedMessageModel::SwapWithoutSignal(int left, int right) {
  R_EXPECT_V(left != right) << "Swapping same element";
  BasicRepeatedModel<Message>::SwapWithoutSignal(left, right);
  std::swap(_subModels[left], _subModels[right]);
}

void RepeatedMessageModel::AppendNewWithoutSignal() {
  auto m = field_ref_.NewMessage();
  field_ref_.Add(*m);
  _subModels.append(new MessageModel(this, m, _subModels.size()));
}

void RepeatedMessageModel::RemoveLastNRowsWithoutSignal(int n) {
  BasicRepeatedModel<Message>::RemoveLastNRowsWithoutSignal(n);
  _subModels.resize(_subModels.size() - n);
}

void RepeatedMessageModel::ClearWithoutSignal() {
  field_ref_.Clear();
  _subModels.clear();
}

bool RepeatedMessageModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  R_EXPECT(index.row() >= 0 && index.row() < _subModels.size(), false) <<
    "Supplied row was out of bounds:" << index.row();
  return _subModels[index.row()]->setData(_subModels[index.row()]->index(index.column()), value, role);
}

QModelIndex RepeatedMessageModel::insert(const Message &message, int row) {
  R_EXPECT(insertRows(row, 1), QModelIndex()) <<  "Insert message failed";
  auto m = GetSubModel(row);
  R_EXPECT(m, QModelIndex()) << "Requested submodel is null";
  MessageModel* mm = m->TryCastAsMessageModel();
  R_EXPECT(mm, QModelIndex()) << "Failed to cast to message model";
  mm->ReplaceBuffer(&message);
  return createIndex(row, 0, this);
}

QModelIndex RepeatedMessageModel::duplicate(const QModelIndex &message) {
  // TODO: write me
  qDebug() << "Unimplemented";
  return message;
}

const ProtoModel *RepeatedMessageModel::GetSubModel(const FieldPath &field_path) const {
  if (field_path.repeated_field_index != -1) {
    if (field_path.repeated_field_index < _subModels.size())
      return _subModels[field_path.repeated_field_index]->GetSubModel(field_path.SkipIndex());
    qDebug() << "Attempting to access out-of-bounds repeated index " << field_path.repeated_field_index
             << " of repeated field `" << field_path.fields[0]->full_name().c_str()
             << "` of size " << _subModels.size();
    return nullptr;
  }
  if (field_path) {
    qDebug() << "Attempting to access sub-field `" << field_path.front()->full_name().c_str()
             << "` of repeated field `" << field_->full_name().c_str() << "` without an index";
    return nullptr;
  }
  return this;
}

bool RepeatedMessageModel::SetData(const QVariant &) {
  qDebug() << "Unimplemented: assigning a QVariant to a repeated message field.";
  return false;
}

QVariant RepeatedMessageModel::Data() const {
  QVector<QVariant> vec;
  for (const auto *sub_model : _subModels) vec.push_back(sub_model->Data());
  return QVariant::fromValue(vec);
}

QVariant RepeatedMessageModel::data(const QModelIndex &index, int role) const {
  R_EXPECT(index.row() >= 0 && index.row() < rowCount(), QVariant())
      << "Row index " << index.row() << " is out of bounds (" << rowCount() << " rows total)";
  return _subModels[index.row()]->data(_subModels[index.row()]->index(index.column()), role);
}

int RepeatedMessageModel::columnCount(const QModelIndex & /*parent*/) const {
  return field_->message_type()->field_count();
}

Qt::ItemFlags RepeatedMessageModel::flags(const QModelIndex &index) const {
  R_EXPECT(index.row() >= 0 && index.row() < _subModels.size(), RepeatedModel::flags(index)) <<
    "Supplied row was out of bounds:" << index.row();

  return _subModels[index.row()]->flags(_subModels[index.row()]->index(index.column()));
}

const std::string &RepeatedMessageModel::MessageName() const {
  auto *msg = field_->message_type();
  if (!msg) {
    static const std::string kSentinel;
    qDebug() << "Message type of RepeatedMessageField is null! This should never happen!";
    return kSentinel;
  }
  return msg->full_name();
}
