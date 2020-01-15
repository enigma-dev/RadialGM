#include "ProtoModel.h"
#include "MessageModel.h"
#include "RepeatedImageModel.h"
#include "RepeatedMessageModel.h"
#include "RepeatedModel.h"

#include "Components/Logger.h"

using namespace google::protobuf;
using CppType = FieldDescriptor::CppType;

/*ProtoModel::ProtoModel(Message *protobuf, QObject *parent) : ProtoModel(protobuf, static_cast<ProtoModelPtr>(nullptr)) {
  QAbstractItemModel::setParent(parent);
}*/

ProtoModel::ProtoModel(ProtoModelPtr parent)
    : QAbstractItemModel(parent), _dirty(false), /*_protobuf(protobuf),*/ _parentModel(parent), _modelBackup(nullptr) {
  connect(this, &ProtoModel::dataChanged, this,
          [this](const QModelIndex &topLeft, const QModelIndex &bottomRight,
                 const QVariant & /*oldValue*/ = QVariant(0), const QVector<int> &roles = QVector<int>()) {
            emit QAbstractItemModel::dataChanged(topLeft, bottomRight, roles);
          });

  RebuildSubModels();
}

ProtoModelPtr ProtoModel::BackupModel(QObject *parent) {
  _backupProtobuf.reset(_protobuf->New());
  _backupProtobuf->CopyFrom(*_protobuf);
  //_modelBackup = new ProtoModel(_backupProtobuf.get(), parent);
  return _modelBackup;
}

ProtoModelPtr ProtoModel::GetBackupModel() { return _modelBackup; }

void ProtoModel::ReplaceBuffer(Message *buffer) {
  beginResetModel();
  SetDirty(true);
  _protobuf->CopyFrom(*buffer);
  RebuildSubModels();
  endResetModel();
}

bool ProtoModel::RestoreBackup() {
  if (_modelBackup == nullptr) return false;
  ReplaceBuffer(_modelBackup->GetBuffer());
  return true;
}

Message *ProtoModel::GetBuffer() { return _protobuf; }

void ProtoModel::SetDirty(bool dirty) { _dirty = dirty; }

bool ProtoModel::IsDirty() { return _dirty; }

int ProtoModel::columnCount(const QModelIndex & /*parent*/) const { return 1; }

bool ProtoModel::SetData(const QVariant &value, int row, int column) {
  return setData(this->index(row, column, QModelIndex()), value);
}

bool ProtoModel::setData(const QModelIndex &index, const QVariant &value, int role) {
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

  emit dataChanged(index, index, oldValue);
  ParentDataChanged();

  return true;
}

QVariant ProtoModel::Data(int row, int column) const {
  return data(this->index(row, column, QModelIndex()), Qt::DisplayRole);
}

QModelIndex ProtoModel::index(int row, int column, const QModelIndex & /*parent*/) const {
  return this->createIndex(row, column, static_cast<void *>(_protobuf));
}

Qt::ItemFlags ProtoModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return nullptr;
  auto flags = QAbstractItemModel::flags(index);
  if (index.row() > 0) flags |= Qt::ItemIsEditable;
  return flags;
}

void ProtoModel::ParentDataChanged() {
  ProtoModelPtr m = GetParentModel();
  while (m != nullptr) {
    emit m->dataChanged(QModelIndex(), QModelIndex());
    m->SetDirty(true);
    m = m->GetParentModel();
  }
}

void UpdateReferences(ProtoModelPtr model, const QString &type, const QString &oldName, const QString &newName) {
  if (model == nullptr) return;

  int rows = model->rowCount();
  for (int row = 0; row < rows; row++) {
    Message *protobuf = model->GetBuffer();

    const Descriptor *desc = protobuf->GetDescriptor();
    const FieldDescriptor *field = desc->FindFieldByNumber(row);
    if (field != nullptr) {
      if (field->cpp_type() == CppType::CPPTYPE_MESSAGE) {
        if (field->is_repeated()) {
          RepeatedMessageModel *rm = model->GetSubModel<RepeatedMessageModel *>(row);
          int cols = rm->rowCount();
          for (int col = 0; col < cols; col++) {
            UpdateReferences(rm->GetSubModel<MessageModel *>(col), type, oldName, newName);
          }
        } else
          UpdateReferences(model->GetSubModel<MessageModel *>(row), type, oldName, newName);
      } else if (field->cpp_type() == CppType::CPPTYPE_STRING && !field->is_repeated()) {
        const QString refType = QString::fromStdString(field->options().GetExtension(buffers::resource_ref));
        if (refType == type && model->Data(row).toString() == oldName) {
          model->SetData(newName, row);
        }
      }
    }
  }
}

QString ResTypeAsString(TypeCase type) {
  switch (type) {
    case TypeCase::kFolder: return "treenode";
    case TypeCase::kBackground: return "background";
    case TypeCase::kFont: return "font";
    case TypeCase::kObject: return "object";
    case TypeCase::kPath: return "path";
    case TypeCase::kRoom: return "room";
    case TypeCase::kSound: return "sound";
    case TypeCase::kSprite: return "sprite";
    case TypeCase::kShader: return "shader";
    case TypeCase::kScript: return "script";
    case TypeCase::kSettings: return "settings";
    case TypeCase::kInclude: return "include";
    case TypeCase::kTimeline: return "timeline";
    case TypeCase::TYPE_NOT_SET: return "unknown";
  }
  return "unknown";
}
