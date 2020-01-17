#include "MessageModel.h"
#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "MainWindow.h"
#include "RepeatedImageModel.h"
#include "RepeatedMessageModel.h"
#include "ResourceModelMap.h"

MessageModel::MessageModel(ProtoModel *parent, Message *protobuf) : ProtoModel(parent, protobuf) { RebuildSubModels(); }

MessageModel::MessageModel(QObject *parent, Message *protobuf) : ProtoModel(parent, protobuf) { RebuildSubModels(); }

void MessageModel::RebuildSubModels() {
  const Descriptor *desc = _protobuf->GetDescriptor();
  const Reflection *refl = _protobuf->GetReflection();
  for (int i = 0; i < desc->field_count(); i++) {
    const FieldDescriptor *field = desc->field(i);

    if (field->cpp_type() == CppType::CPPTYPE_MESSAGE) {
      if (field->is_repeated()) {
        _subModels[field->number()] = new RepeatedMessageModel(this, _protobuf, field);
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
        _subModels[field->number()] = new MessageModel(this, refl->MutableMessage(_protobuf, field));
      }
    } else if (field->cpp_type() == CppType::CPPTYPE_STRING && field->is_repeated()) {
      if (field->name() == "subimages") {
        _subModels[field->number()] = new RepeatedImageModel(this, _protobuf, field);
        GetSubModel<RepeatedImageModel *>(field->number());
      } else
        _subModels[field->number()] = new RepeatedStringModel(this, _protobuf, field);
    }
  }
}

int MessageModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  const Descriptor *desc = _protobuf->GetDescriptor();
  return desc->field_count();
}

int MessageModel::columnCount(const QModelIndex & /*parent*/) const { return 1; }

bool MessageModel::SetData(const QVariant &value, int row, int column) {
  return setData(this->index(row, column, QModelIndex()), value);
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

QVariant MessageModel::Data(int row, int column) const {
  return data(this->index(row, column, QModelIndex()), Qt::DisplayRole);
}

QVariant MessageModel::data(const QModelIndex &index, int role) const {
  R_EXPECT(index.isValid(), QVariant()) << "Supplied index was invalid:" << index;
  if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::DecorationRole) return QVariant();

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

  // These are for icons in things like the room's instance list
  if (role == Qt::DecorationRole) {
    const QString refType = QString::fromStdString(field->options().GetExtension(buffers::resource_ref));
    if (refType == "object") {
      MessageModel *sprModel = GetObjectSprite(data(index, Qt::DisplayRole).toString());
      if (sprModel != nullptr) {
        RepeatedImageModel *subImgs = sprModel->GetSubModel<RepeatedImageModel *>(Sprite::kSubimagesFieldNumber);
        if (subImgs != nullptr && subImgs->rowCount() > 0) {
          return ArtManager::GetIcon(subImgs->Data(0).toString());
        }
      }
    } else if (refType == "background") {
      MessageModel *bkgModel =
          MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, data(index, Qt::DisplayRole).toString());
      if (bkgModel != nullptr) {
        bkgModel = bkgModel->GetSubModel<MessageModel *>(TreeNode::kBackgroundFieldNumber);
        if (bkgModel != nullptr) return ArtManager::GetIcon(bkgModel->Data(Background::kImageFieldNumber).toString());
      }
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

MessageModel *MessageModel::BackupModel(QObject *parent) {
  _backupProtobuf.reset(_protobuf->New());
  _backupProtobuf->CopyFrom(*_protobuf);
  _modelBackup = new MessageModel(parent, _backupProtobuf.get());
  return _modelBackup;
}

MessageModel *MessageModel::GetBackupModel() { return _modelBackup; }

void MessageModel::ReplaceBuffer(Message *buffer) {
  beginResetModel();
  SetDirty(true);
  _protobuf->CopyFrom(*buffer);
  RebuildSubModels();
  endResetModel();
}

bool MessageModel::RestoreBackup() {
  if (_modelBackup == nullptr) return false;
  ReplaceBuffer(_modelBackup->GetBuffer());
  return true;
}

Message *MessageModel::GetBuffer() { return _protobuf; }

void UpdateReferences(MessageModel *model, const QString &type, const QString &oldName, const QString &newName) {
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
