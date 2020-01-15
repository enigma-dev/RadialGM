#include "MessageModel.h"
#include "Components/Logger.h"

#include <google/protobuf/descriptor.h>

using CppType = FieldDescriptor::CppType;

void MessageModel::RebuildSubModels() {
  const Descriptor *desc = _protobuf->GetDescriptor();
  const Reflection *refl = _protobuf->GetReflection();
  for (int i = 0; i < desc->field_count(); i++) {
    const FieldDescriptor *field = desc->field(i);

    if (field->cpp_type() == CppType::CPPTYPE_MESSAGE) {
      if (field->is_repeated()) {
        //subModels.repeatedModels[field->number()] = new RepeatedProtoModel(protobuf, field, this);
      } else {
        const OneofDescriptor *oneof = field->containing_oneof();
        if (oneof) {
          if (refl->HasOneof(*_protobuf, oneof)) {
            field = refl->GetOneofFieldDescriptor(*_protobuf, oneof);
            if (field->cpp_type() != CppType::CPPTYPE_MESSAGE) continue;  // is prolly folder
          } else {
            continue;  // don't allocate if not set
          }
        }
        //subModels.protoModels[field->number()] = new ProtoModel(refl->MutableMessage(protobuf, field), this);
      }
    } else if (field->cpp_type() == CppType::CPPTYPE_STRING && field->is_repeated()) {
      if (field->name() == "subimages")
        ;
      //subModels.repeatedStringModels[field->number()] =
      //new SpriteSubimageModel(refl->GetMutableRepeatedFieldRef<std::string>(_protobuf, field), field, this);
      //else
      //subModels.repeatedStringModels[field->number()] =
      //new RepeatedStringModel(refl->GetMutableRepeatedFieldRef<std::string>(_protobuf, field), field, this);
    }
  }
}

int MessageModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  const Descriptor *desc = _protobuf->GetDescriptor();
  return desc->field_count();
}

int MessageModel::columnCount(const QModelIndex &parent) const {
  return 1;  //TODO:
}

bool MessageModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  R_EXPECT(index.isValid(), false) << "Supplied index was invalid:" << index;

  const Descriptor *desc = _protobuf->GetDescriptor();
  const Reflection *refl = _protobuf->GetReflection();
  const FieldDescriptor *field = desc->FindFieldByNumber(index.row());
  if (!field) return false;

  const QVariant oldValue = this->data(index, role);

  switch (field->cpp_type()) {
    case CppType::CPPTYPE_MESSAGE: {
      break;
    }
    case CppType::CPPTYPE_INT32: refl->SetInt32(_protobuf, field, value.toInt()); break;
    case CppType::CPPTYPE_INT64: refl->SetInt64(_protobuf, field, value.toLongLong()); break;
    case CppType::CPPTYPE_UINT32: refl->SetUInt32(_protobuf, field, value.toUInt()); break;
    case CppType::CPPTYPE_UINT64: refl->SetUInt64(_protobuf, field, value.toULongLong()); break;
    case CppType::CPPTYPE_DOUBLE: refl->SetDouble(_protobuf, field, value.toDouble()); break;
    case CppType::CPPTYPE_FLOAT: refl->SetFloat(_protobuf, field, value.toFloat()); break;
    case CppType::CPPTYPE_BOOL: refl->SetBool(_protobuf, field, value.toBool()); break;
    case CppType::CPPTYPE_ENUM:
      refl->SetEnum(_protobuf, field, field->enum_type()->FindValueByNumber(value.toInt()));
      break;
    case CppType::CPPTYPE_STRING: refl->SetString(_protobuf, field, value.toString().toStdString()); break;
  }

  SetDirty(true);
  emit dataChanged(index, index, oldValue);
  ParentDataChanged();

  return true;
}

QVariant MessageModel::data(const QModelIndex &index, int role) const {
  R_EXPECT(index.isValid(), QVariant()) << "Supplied index was invalid:" << index;
  if (role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();

  const Descriptor *desc = _protobuf->GetDescriptor();
  const Reflection *refl = _protobuf->GetReflection();
  const FieldDescriptor *field = desc->FindFieldByNumber(index.row());

  if (!field) {
    // Proto fields always start at one. So this bit of a hack for displaying data in a table
    if (index.row() == 0 && role == Qt::DisplayRole) {
      return tr("Value");
    }
    return QVariant();
  }

  // If the field has't been initialized return an invalid QVariant. (see QVariant.isValid())
  if (!refl->HasField(*_protobuf, field)) return QVariant();

  switch (field->cpp_type()) {
    case CppType::CPPTYPE_MESSAGE: R_EXPECT(false, QVariant()) << "The requested field " << index << " is a message";
    case CppType::CPPTYPE_INT32: return refl->GetInt32(*_protobuf, field);
    case CppType::CPPTYPE_INT64: return static_cast<long long>(refl->GetInt64(*_protobuf, field));
    case CppType::CPPTYPE_UINT32: return refl->GetUInt32(*_protobuf, field);
    case CppType::CPPTYPE_UINT64: return static_cast<unsigned long long>(refl->GetUInt64(*_protobuf, field));
    case CppType::CPPTYPE_DOUBLE: return refl->GetDouble(*_protobuf, field);
    case CppType::CPPTYPE_FLOAT: return refl->GetFloat(*_protobuf, field);
    case CppType::CPPTYPE_BOOL: return refl->GetBool(*_protobuf, field);
    case CppType::CPPTYPE_ENUM: return refl->GetEnumValue(*_protobuf, field);
    case CppType::CPPTYPE_STRING: return QString::fromStdString(refl->GetString(*_protobuf, field));
  }

  return QVariant();
}

QModelIndex MessageModel::parent(const QModelIndex & /*index*/) const { return QModelIndex(); }

QVariant MessageModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();

  // Proto fields always start at one. So this bit of a hack for displaying data in a table
  if (section == 0) return tr("Property");

  const Descriptor *desc = _protobuf->GetDescriptor();
  const FieldDescriptor *field = desc->FindFieldByNumber(section);

  if (field != nullptr) return QString::fromStdString(field->name());

  return "";
}

QModelIndex MessageModel::index(int row, int column, const QModelIndex & /*parent*/) const {
  return this->createIndex(row, column, static_cast<void *>(_protobuf));
}
