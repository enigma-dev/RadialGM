#include "ProtoModel.h"

ProtoModel::ProtoModel(google::protobuf::Message *protobuf, QObject *parent)
    : QAbstractItemModel(parent), dirty(false), protobuf(protobuf) {
  protobufBackup = protobuf->New();
  protobufBackup->CopyFrom(*protobuf);
}

void ProtoModel::ReplaceBuffer(google::protobuf::Message *buffer) {
  SetDirty(true);
  protobuf->CopyFrom(*buffer);
  emit dataChanged(index(0), index(rowCount()));
}

void ProtoModel::RestoreBuffer() {
  std::swap(protobuf, protobufBackup);
  protobufBackup->CopyFrom(*protobuf);
}

int ProtoModel::rowCount(const QModelIndex & /*parent*/) const {
  const google::protobuf::Descriptor *desc = protobuf->GetDescriptor();
  return desc->field_count();
}

void ProtoModel::SetDirty(bool dirty) { this->dirty = dirty; }

bool ProtoModel::IsDirty() { return dirty; }

int ProtoModel::columnCount(const QModelIndex & /*parent*/) const { return 1; }

bool ProtoModel::setData(const QModelIndex &index, const QVariant &value, int /*role*/) {
  const google::protobuf::Descriptor *desc = protobuf->GetDescriptor();
  const google::protobuf::Reflection *refl = protobuf->GetReflection();
  const google::protobuf::FieldDescriptor *field = desc->FindFieldByNumber(index.row());
  if (!field) return false;

  switch (field->cpp_type()) {
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE: {
      break;
    }
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
      refl->SetInt32(protobuf, field, value.toInt());
      break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
      refl->SetInt64(protobuf, field, value.toLongLong());
      break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
      refl->SetUInt32(protobuf, field, value.toUInt());
      break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
      refl->SetUInt64(protobuf, field, value.toULongLong());
      break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
      refl->SetDouble(protobuf, field, value.toDouble());
      break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
      refl->SetFloat(protobuf, field, value.toFloat());
      break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
      refl->SetBool(protobuf, field, value.toBool());
      break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
      refl->SetEnum(protobuf, field, field->enum_type()->FindValueByNumber(value.toInt()));
      break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
      refl->SetString(protobuf, field, value.toString().toStdString());
      break;
  }

  SetDirty(true);
  emit dataChanged(index, index);
  return true;
}

QVariant ProtoModel::data(int index) const { return data(this->index(index, 0, QModelIndex()), Qt::DisplayRole); }

QVariant ProtoModel::data(const QModelIndex &index, int role) const {
  if (role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();

  const google::protobuf::Descriptor *desc = protobuf->GetDescriptor();
  const google::protobuf::Reflection *refl = protobuf->GetReflection();
  const google::protobuf::FieldDescriptor *field = desc->FindFieldByNumber(index.row());
  if (!field) return QVariant();

  switch (field->cpp_type()) {
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
      return refl->GetInt32(*protobuf, field);
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
      return refl->GetInt32(*protobuf, field);
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
      return static_cast<long long>(refl->GetInt64(*protobuf, field));
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
      return refl->GetUInt32(*protobuf, field);
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
      return static_cast<unsigned long long>(refl->GetUInt64(*protobuf, field));
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
      return refl->GetDouble(*protobuf, field);
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
      return refl->GetFloat(*protobuf, field);
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
      return refl->GetBool(*protobuf, field);
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
      return refl->GetInt32(*protobuf, field);
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
      return refl->GetString(*protobuf, field).c_str();
  }

  return QVariant();
}

QModelIndex ProtoModel::parent(const QModelIndex & /*index*/) const { return QModelIndex(); }

QVariant ProtoModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  return "hey";
}

QModelIndex ProtoModel::index(int row, int column, const QModelIndex & /*parent*/) const {
  return this->createIndex(row, column);
}

Qt::ItemFlags ProtoModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return nullptr;

  return QAbstractItemModel::flags(index);
}
