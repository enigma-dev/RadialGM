#include "ProtoModel.h"
#include "RepeatedProtoModel.h"

#include "Components/Logger.h"

#include <iostream>

using namespace google::protobuf;
using CppType = FieldDescriptor::CppType;

ProtoModel::ProtoModel(Message *protobuf, QObject *parent)
    : QAbstractItemModel(parent), dirty(false), protobuf(protobuf) {
  connect(this, &ProtoModel::dataChanged, this,
          [this](const QModelIndex &topLeft, const QModelIndex &bottomRight,
                 const QVariant & /*oldValue*/ = QVariant(0), const QVector<int> &roles = QVector<int>()) {
            emit QAbstractItemModel::dataChanged(topLeft, bottomRight, roles);
          });

  const Descriptor *desc = protobuf->GetDescriptor();
  const Reflection *refl = protobuf->GetReflection();
  for (int i = 0; i < desc->field_count(); i++) {
    const google::protobuf::FieldDescriptor *field = desc->field(i);

    if (field->cpp_type() == CppType::CPPTYPE_MESSAGE) {
      if (field->is_repeated()) {
        repeatedModels[field->number()] = new RepeatedProtoModel(protobuf, field, this);
        for (int j = 0; j < refl->FieldSize(*protobuf, field); j++) {
          ProtoModel *subModel = new ProtoModel(refl->MutableRepeatedMessage(protobuf, field, j), this);
          repeatedMessages[field->number()].append(QVariant::fromValue(static_cast<void *>(subModel)));
        }
      } else {
        const google::protobuf::OneofDescriptor *oneof = field->containing_oneof();
        if (oneof) {
          if (refl->HasOneof(*protobuf, oneof)) {
            field = refl->GetOneofFieldDescriptor(*protobuf, oneof);
            if (field->cpp_type() != CppType::CPPTYPE_MESSAGE) continue;  // is prolly folder
          } else {
            continue;  // don't allocate if not set
          }
        }
        ProtoModel *subModel = new ProtoModel(refl->MutableMessage(protobuf, field), this);
        messages[field->number()] = QVariant::fromValue(static_cast<void *>(subModel));
      }
    } else if (field->cpp_type() == CppType::CPPTYPE_STRING && field->is_repeated()) {
      for (int j = 0; j < refl->FieldSize(*protobuf, field); j++) {
        repeatedMessages[field->number()].append(QString::fromStdString(refl->GetRepeatedString(*protobuf, field, j)));
      }
    }
  }
}

ProtoModel::~ProtoModel() {}

ProtoModel* ProtoModel::Copy(QObject *parent) {
  google::protobuf::Message *protobufCopy = protobuf->New();
  protobufCopy->CopyFrom(*protobuf);
  return new ProtoModel(protobufCopy, parent);
}

void ProtoModel::ReplaceBuffer(Message *buffer) {
  SetDirty(true);
  protobuf->CopyFrom(*buffer);
  emit dataChanged(index(0), index(rowCount()));
}

google::protobuf::Message *ProtoModel::GetBuffer() { return protobuf; }

int ProtoModel::rowCount(const QModelIndex & /*parent*/) const {
  const Descriptor *desc = protobuf->GetDescriptor();
  return desc->field_count();
}

void ProtoModel::SetDirty(bool dirty) { this->dirty = dirty; }

bool ProtoModel::IsDirty() { return dirty; }

int ProtoModel::columnCount(const QModelIndex & /*parent*/) const { return 1; }

bool ProtoModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  R_EXPECT(index.isValid(), false) << "Supplied index was invalid:" << index;

  const Descriptor *desc = protobuf->GetDescriptor();
  const Reflection *refl = protobuf->GetReflection();
  const FieldDescriptor *field = desc->FindFieldByNumber(index.row());
  if (!field) return false;

  const QVariant oldValue = this->data(index, role);

  switch (field->cpp_type()) {
    case CppType::CPPTYPE_MESSAGE: {
      break;
    }
    case CppType::CPPTYPE_INT32:
      refl->SetInt32(protobuf, field, value.toInt());
      break;
    case CppType::CPPTYPE_INT64:
      refl->SetInt64(protobuf, field, value.toLongLong());
      break;
    case CppType::CPPTYPE_UINT32:
      refl->SetUInt32(protobuf, field, value.toUInt());
      break;
    case CppType::CPPTYPE_UINT64:
      refl->SetUInt64(protobuf, field, value.toULongLong());
      break;
    case CppType::CPPTYPE_DOUBLE:
      refl->SetDouble(protobuf, field, value.toDouble());
      break;
    case CppType::CPPTYPE_FLOAT:
      refl->SetFloat(protobuf, field, value.toFloat());
      break;
    case CppType::CPPTYPE_BOOL:
      refl->SetBool(protobuf, field, value.toBool());
      break;
    case CppType::CPPTYPE_ENUM:
      refl->SetEnum(protobuf, field, field->enum_type()->FindValueByNumber(value.toInt()));
      break;
    case CppType::CPPTYPE_STRING:
      refl->SetString(protobuf, field, value.toString().toStdString());
      break;
  }

  SetDirty(true);
  emit dataChanged(index, index, oldValue);
  return true;
}

QVariant ProtoModel::data(int row, int column) const {
  return data(this->index(row, column, QModelIndex()), Qt::DisplayRole);
}

QVariant ProtoModel::data(const QModelIndex &index, int role) const {
  R_EXPECT(index.isValid(), QVariant()) << "Supplied index was invalid:" << index;
  if (role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();

  const Descriptor *desc = protobuf->GetDescriptor();
  const Reflection *refl = protobuf->GetReflection();
  const FieldDescriptor *field = desc->FindFieldByNumber(index.row());
  if (!field) {
    if (index.row() == 0 && role == Qt::DisplayRole) {
      return "Value";
    }
    return QVariant();
  }

  switch (field->cpp_type()) {
    case CppType::CPPTYPE_MESSAGE:
      if (field->is_repeated())
        return repeatedMessages[index.row()][index.column()];
      else
        return messages[index.row()];
    case CppType::CPPTYPE_INT32:
      return refl->GetInt32(*protobuf, field);
    case CppType::CPPTYPE_INT64:
      return static_cast<long long>(refl->GetInt64(*protobuf, field));
    case CppType::CPPTYPE_UINT32:
      return refl->GetUInt32(*protobuf, field);
    case CppType::CPPTYPE_UINT64:
      return static_cast<unsigned long long>(refl->GetUInt64(*protobuf, field));
    case CppType::CPPTYPE_DOUBLE:
      return refl->GetDouble(*protobuf, field);
    case CppType::CPPTYPE_FLOAT:
      return refl->GetFloat(*protobuf, field);
    case CppType::CPPTYPE_BOOL:
      return refl->GetBool(*protobuf, field);
    case CppType::CPPTYPE_ENUM:
      return refl->GetInt32(*protobuf, field);
    case CppType::CPPTYPE_STRING:
      return QString::fromStdString(refl->GetString(*protobuf, field));
  }

  return QVariant();
}

RepeatedProtoModel *ProtoModel::GetRepeatedSubModel(int fieldNum) { return repeatedModels[fieldNum]; }

ProtoModel *ProtoModel::GetSubModel(int fieldNum) {
  return static_cast<ProtoModel *>(this->data(fieldNum).value<void *>());
}

ProtoModel *ProtoModel::GetSubModel(int fieldNum, int index) {
  if (repeatedMessages.contains(fieldNum) && repeatedMessages[fieldNum].count() > index)
   return static_cast<ProtoModel *>(repeatedMessages[fieldNum][index].value<void *>());
  else
    return nullptr;
}

QString ProtoModel::GetString(int fieldNum, int index) const {
  if (repeatedMessages.contains(fieldNum) && repeatedMessages[fieldNum].count() > index)
    return repeatedMessages[fieldNum][index].toString();
  else
    return "";
}

QModelIndex ProtoModel::parent(const QModelIndex & /*index*/) const { return QModelIndex(); }

QVariant ProtoModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  if (section == 0) return tr("Property");

  const Descriptor *desc = protobuf->GetDescriptor();
  const FieldDescriptor *field = desc->FindFieldByNumber(section);

  if (field != nullptr) return QString::fromStdString(field->name());

  return "";
}

QModelIndex ProtoModel::index(int row, int column, const QModelIndex & /*parent*/) const {
  return this->createIndex(row, column, static_cast<void *>(protobuf));
}

Qt::ItemFlags ProtoModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return nullptr;
  auto flags = QAbstractItemModel::flags(index);
  if (index.row() > 0) flags |= Qt::ItemIsEditable;
  return flags;
}

void UpdateReferences(ProtoModel* model, const QString& type, const QString& oldName, const QString& newName) {

  if (model == nullptr) return;

  int rows = model->rowCount();
  for (int row = 0; row < rows; row++) {

    google::protobuf::Message* protobuf = model->GetBuffer();

    const Descriptor *desc = protobuf->GetDescriptor();
    const Reflection *refl = protobuf->GetReflection();
    const FieldDescriptor *field = desc->FindFieldByNumber(row);
    if (field != nullptr) {
      if (field->cpp_type() == CppType::CPPTYPE_MESSAGE) {
        if (field->is_repeated()) {
          int cols = model->GetRepeatedSubModel(row)->columnCount();
          for (int col = 0; col < cols; col++) {
            UpdateReferences(model->GetSubModel(row, col), type, oldName, newName);
          }
        } else UpdateReferences(model->GetSubModel(row), type, oldName, newName);
      } else if (field->cpp_type() == CppType::CPPTYPE_STRING && !field->is_repeated()) {
        const QString refType = QString::fromStdString(field->options().GetExtension(buffers::resource_ref));
        if (refType == type && model->data(row, 0).toString() == oldName) {
          //qDebug() << "renamed " << oldName << " to " << newName;
          model->setData(model->index(row, 0, QModelIndex()), newName, Qt::DisplayRole);
        }
      }
    }
  }
}

QString ResTypeAsString(TypeCase type) {
  switch (type) {
    case TypeCase::kFolder:       return "treenode";
    case TypeCase::kBackground:   return "background";
    case TypeCase::kFont:         return "font";
    case TypeCase::kObject:       return "object";
    case TypeCase::kPath:         return "path";
    case TypeCase::kRoom:         return "room";
    case TypeCase::kSound:        return "sound";
    case TypeCase::kSprite:       return "sprite";
    case TypeCase::kTimeline:     return "timeline";
  }
  return "unknown";
}
