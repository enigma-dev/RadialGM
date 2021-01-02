#include "MessageModel.h"
#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "RepeatedImageModel.h"
#include "RepeatedMessageModel.h"
#include "ResourceModelMap.h"

MessageModel::MessageModel(ProtoModel *parent, Message *protobuf)
    : ProtoModel(parent, protobuf->GetDescriptor()->name()), _protobuf(protobuf),
      descriptor_(protobuf->GetDescriptor()) { RebuildSubModels(); }

MessageModel::MessageModel(QObject *parent, Message *protobuf)
    : ProtoModel(parent, protobuf->GetDescriptor()->name()), _protobuf(protobuf),
      descriptor_(protobuf->GetDescriptor()) { RebuildSubModels(); }

MessageModel::MessageModel(QObject *parent, const Descriptor *descriptor)
    : ProtoModel(parent, descriptor->name()), _protobuf(nullptr), descriptor_(descriptor) {}

const FieldDescriptor *MessageModel::GetRowDescriptor(int row) const {
  if (row < 0 || row >= descriptor_->field_count()) {
    qDebug() << "Requesting descriptor of invalid row " << row
             << " of MessageModel " << descriptor_->full_name().c_str();
    return nullptr;
  }
  return descriptor_->field(row);
}

bool IsCulledOneof_(const Reflection *refl, const Message &message, const FieldDescriptor *field) {
  const OneofDescriptor *oneof = field->containing_oneof();
  return (oneof && refl->HasOneof(message, oneof) && !refl->HasField(message, field));
}

bool MessageModel::IsCulledOneofRow(int row) const {
  return IsCulledOneof_(_protobuf->GetReflection(), *_protobuf, descriptor_->field(row));
}

void MessageModel::RebuildSubModels() {
  submodels_by_field_.clear();
  submodels_by_row_.clear();
  if (!_protobuf) {
    qDebug() << "Whatever, man.";
    return;
  }

  const Descriptor *desc = _protobuf->GetDescriptor();
  const Reflection *refl = _protobuf->GetReflection();
  submodels_by_row_.resize(desc->field_count());

  for (int i = 0; i < desc->field_count(); i++) {
    const FieldDescriptor *field = desc->field(i);

    if (field->cpp_type() == CppType::CPPTYPE_MESSAGE) {
      if (field->is_repeated()) {
        submodels_by_field_[field->number()] = submodels_by_row_[i] =
            new RepeatedMessageModel(this, _protobuf, field);
      } else {
        // Ignore all unset oneof fields if any is set
        if (IsCulledOneof_(refl, *_protobuf, field)) continue;
        // Only recursively build fields if they're set
        if (refl->HasField(*_protobuf, field)) {
          submodels_by_field_[field->number()] = submodels_by_row_[i] =
              new MessageModel(this, refl->MutableMessage(_protobuf, field));
        } else {
          submodels_by_field_[field->number()] = submodels_by_row_[i] = new MessageModel(this, field->message_type());
        }
      }
    } else if (field->cpp_type() == CppType::CPPTYPE_STRING && field->is_repeated()) {
      if (field->options().GetExtension(buffers::file_kind) == buffers::FileKind::IMAGE) {
        submodels_by_field_[field->number()] = submodels_by_row_[i] = new RepeatedImageModel(this, _protobuf, field);
        GetSubModel<RepeatedImageModel *>(field->number());
      } else {
        submodels_by_field_[field->number()] = submodels_by_row_[i] = new RepeatedStringModel(this, _protobuf, field);
      }
    }
  }
}

int MessageModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return descriptor_->field_count();
}

int MessageModel::columnCount(const QModelIndex & /*parent*/) const { return 1; }

bool MessageModel::SetData(const QVariant &value, int row, int column) {
  return setData(this->index(row, column, QModelIndex()), value);
}

bool MessageModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  R_EXPECT(index.isValid(), false) << "Supplied index was invalid:" << index;

  const Descriptor *desc = _protobuf->GetDescriptor();
  const Reflection *refl = _protobuf->GetReflection();
  const FieldDescriptor *field = desc->field(index.row() - 1);
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
    case CppType::CPPTYPE_BOOL: {
      if (role == Qt::CheckStateRole) {
        auto checked = ((Qt::CheckState)value.toInt() == Qt::Checked);
        refl->SetBool(_protobuf, field, checked ? true : false);
      } else {
        refl->SetBool(_protobuf, field, value.toBool());
      }
      break;
    }
    case CppType::CPPTYPE_ENUM:
      refl->SetEnum(_protobuf, field, field->enum_type()->FindValueByNumber(value.toInt()));
      break;
    case CppType::CPPTYPE_STRING: refl->SetString(_protobuf, field, value.toString().toStdString()); break;
  }

  SetDirty(true);
  emit DataChanged(index, index, oldValue);
  ParentDataChanged();

  return true;
}

bool MessageModel::SetData(const FieldPath &field_path, const QVariant &value) {
  if (!field_path) {
    qDebug() << "Unimplemented: Attempting to assign QVariant to a message.";
    return false;
  }
  if (field_path.fields.size() > 1) {
    auto smit = submodels_by_field_.find(field_path.front()->number());
    if (smit == submodels_by_field_.end()) return false;
    return smit.value()->SetData(field_path.SubPath(1), value);
  }
  return SetData(value, field_path.front()->number());
}

QVariant MessageModel::Data(const FieldPath &field_path) const {
  if (!_protobuf) return {};
  if (field_path.fields.empty()) {
    return QVariant::fromValue(AbstractMessage(*_protobuf));
  }
  int row = field_path.front()->index();
  if (field_path.size() == 1) {
    if (field_path.front().repeated_field_index >= 0) {
      return submodels_by_row_[row]->Data(field_path.front().repeated_field_index);
    } else {
      return Data(row);
    }
  }
  if (field_path.front().repeated_field_index >= 0) {
    const auto *rmm = submodels_by_row_[row]->As<RepeatedMessageModel>();
    if (rmm) {
      return rmm->GetSubModel<ProtoModel>(field_path.front().repeated_field_index)->Data(field_path.SubPath(1));
    } else {
      qDebug() << "Internal error: Intermediate field indicated by FieldPath is not a RepeatedMessageModel...";
    }
  }
  return submodels_by_row_[row]->Data(field_path.SubPath(1));
}

QVariant MessageModel::Data(int row, int column) const {
  return data(this->index(row, column, QModelIndex()), Qt::DisplayRole);
}

template<bool NO_DEFAULT>
QVariant MessageModel::dataInternal(const QModelIndex &index, int role) const {
  if (!_protobuf) return {};
  R_EXPECT(index.isValid(), QVariant()) << "Supplied index was invalid:" << index;
  if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::DecorationRole &&
      role != Qt::CheckStateRole) return QVariant();

  const Descriptor *desc = descriptor_;
  const Reflection *refl = _protobuf->GetReflection();

  if (index.row() < 0 || index.row() >= desc->field_count()) {
    qDebug() << "Accessing out-of-range proto row " << index.row() << " of " << desc->field_count();
    return QVariant();
  }
  const FieldDescriptor *field = desc->field(index.row());

  // These are for icons in things like the room's instance list
  if (role == Qt::DecorationRole) {
    // TODO: Move the decoration attributes out of TreeModel::FieldMeta and into a base struct shared by ProtoModel.
    return QVariant();
  }

  // The logic below will kill proto if the field is repeated. Abort now.
  if (field->is_repeated()) {
     qDebug() << "The requested field " << index.row() << " (" << field->name().c_str() << ") is repeated...";
     return QVariant();
  }
  // If the field has't been initialized return an invalid QVariant. (see QVariant.isValid())
  if (NO_DEFAULT && !refl->HasField(*_protobuf, field)) return QVariant();

  auto cpp_type = field->cpp_type();
  if (role == Qt::CheckStateRole) {
    if (cpp_type != CppType::CPPTYPE_BOOL) return QVariant();
    auto value = refl->GetBool(*_protobuf, field);
    return value ? Qt::Checked : Qt::Unchecked;
  }

  switch (cpp_type) {
    case CppType::CPPTYPE_MESSAGE: R_EXPECT(false, QVariant())
        << "The requested field " << field->full_name().c_str() << " is a message";
    case CppType::CPPTYPE_INT32: return refl->GetInt32(*_protobuf, field);
    case CppType::CPPTYPE_INT64: return static_cast<long long>(refl->GetInt64(*_protobuf, field));
    case CppType::CPPTYPE_UINT32: return refl->GetUInt32(*_protobuf, field);
    case CppType::CPPTYPE_UINT64: return static_cast<unsigned long long>(refl->GetUInt64(*_protobuf, field));
    case CppType::CPPTYPE_DOUBLE: return refl->GetDouble(*_protobuf, field);
    case CppType::CPPTYPE_FLOAT: return refl->GetFloat(*_protobuf, field);
    case CppType::CPPTYPE_BOOL: return QVariant();
    case CppType::CPPTYPE_ENUM: return refl->GetEnumValue(*_protobuf, field);
    case CppType::CPPTYPE_STRING: return QString::fromStdString(refl->GetString(*_protobuf, field));
  }

  return QVariant();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const {
  return dataInternal<true>(index, role);
}

QVariant MessageModel::dataOrDefault(const QModelIndex &index, int role) const {
  return dataInternal<false>(index, role);
}

QModelIndex MessageModel::parent(const QModelIndex & /*index*/) const { return QModelIndex(); }

QVariant MessageModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();

  const Descriptor *desc = _protobuf->GetDescriptor();
  const FieldDescriptor *field = desc->field(section);

  if (field != nullptr) return QString::fromStdString(field->name());

  return "";
}

QModelIndex MessageModel::index(int row, int column, const QModelIndex & /*parent*/) const {
  return this->createIndex(row, column, static_cast<void *>(_protobuf));
}

Qt::ItemFlags MessageModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return Qt::NoItemFlags;
  auto flags = QAbstractItemModel::flags(index);
  // Row 0 isn't a valid field in messages. We use it as header data
  if (index.row() > 0) {
    auto datas = data(index, Qt::CheckStateRole);
    flags |= (datas.isValid()) ? Qt::ItemIsUserCheckable : Qt::ItemIsEditable;
  }
  return flags;
}

MessageModel *MessageModel::BackupModel(QObject *parent) {
  if (!_protobuf) return nullptr;
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
  qDebug() << "Buffer replaced; rebuilding submodels";
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
    const FieldDescriptor *field = desc->field(row);
    if (field != nullptr) {
      if (field->cpp_type() == CppType::CPPTYPE_MESSAGE) {
        if (field->is_repeated()) {
          RepeatedMessageModel *rm = model->GetSubModel<RepeatedMessageModel>(row);
          int cols = rm->rowCount();
          for (int col = 0; col < cols; col++) {
            UpdateReferences(rm->GetSubModel<MessageModel>(col), type, oldName, newName);
          }
        } else
          UpdateReferences(model->GetSubModel<MessageModel>(row), type, oldName, newName);
      } else if (field->cpp_type() == CppType::CPPTYPE_STRING && !field->is_repeated()) {
        const QString refType = QString::fromStdString(field->options().GetExtension(buffers::resource_ref));
        if (refType == type && model->Data(row).toString() == oldName) {
          model->SetData(newName, row);
        }
      }
    }
  }
}
