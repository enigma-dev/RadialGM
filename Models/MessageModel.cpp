#include "MessageModel.h"
#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "RepeatedModel.h"
#include "RepeatedMessageModel.h"
#include "ResourceModelMap.h"

MessageModel::MessageModel(ProtoModel *parent, Message *protobuf)
    : ProtoModel(parent, protobuf->GetDescriptor()->name(), protobuf->GetDescriptor()), _protobuf(protobuf)
      { RebuildSubModels(); }

MessageModel::MessageModel(QObject *parent, Message *protobuf)
    : ProtoModel(parent, protobuf->GetDescriptor()->name(), protobuf->GetDescriptor()), _protobuf(protobuf)
      { RebuildSubModels(); }

MessageModel::MessageModel(QObject *parent, const Descriptor *descriptor)
    : ProtoModel(parent, descriptor->name(), descriptor), _protobuf(nullptr) {}

QString MessageModel::GetDisplayName() const {
  auto& display = GetMessageDisplay(GetDescriptor()->full_name());
  if (display.isValid) return Data(display.label_field).toString();
  else return QString::fromStdString(GetDescriptor()->name());
}

QIcon MessageModel::GetDisplayIcon() const {
  auto& display = GetMessageDisplay(GetDescriptor()->full_name());
  if (display.isValid) {
    if (display.icon_field) {
      if (const ProtoModel *icon_field = GetSubModel(display.icon_field))
        return icon_field->GetDisplayIcon();
    }
    if (display.icon_lookup_function) return display.icon_lookup_function(this);
    if (!display.default_icon_name.isEmpty()) return ArtManager::GetIcon(display.default_icon_name);
  }
  return {};
}

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
  R_EXPECT_V(_protobuf) << "Internal protobuf null";

  const Descriptor *desc = _protobuf->GetDescriptor();
  const Reflection *refl = _protobuf->GetReflection();
  submodels_by_row_.resize(desc->field_count());

  for (int i = 0; i < desc->field_count(); i++) {
    const FieldDescriptor *field = desc->field(i);

    if (field->is_repeated()) {
      switch (field->cpp_type()) {
        case CppType::CPPTYPE_ENUM: {
          qDebug() << "ENUMs not yet handled";
          break;
        }
        case CppType::CPPTYPE_MESSAGE: {
          submodels_by_field_[field->number()] = submodels_by_row_[i] = new RepeatedMessageModel(this, _protobuf, field);
          break;
        }
        case CppType::CPPTYPE_BOOL: {
          submodels_by_field_[field->number()] = submodels_by_row_[i] = new RepeatedBoolModel(this, _protobuf, field);
          break;
        }
        case CppType::CPPTYPE_INT32: {
          submodels_by_field_[field->number()] = submodels_by_row_[i] = new RepeatedInt32Model(this, _protobuf, field);
          break;
        }
        case CppType::CPPTYPE_INT64: {
          submodels_by_field_[field->number()] = submodels_by_row_[i] = new RepeatedInt64Model(this, _protobuf, field);
          break;
        }
        case CppType::CPPTYPE_UINT32: {
          submodels_by_field_[field->number()] = submodels_by_row_[i] = new RepeatedUInt32Model(this, _protobuf, field);
          break;
        }
        case CppType::CPPTYPE_UINT64: {
          submodels_by_field_[field->number()] = submodels_by_row_[i] = new RepeatedUInt64Model(this, _protobuf, field);
          break;
        }
        case CppType::CPPTYPE_FLOAT: {
          submodels_by_field_[field->number()] = submodels_by_row_[i] = new RepeatedFloatModel(this, _protobuf, field);
          break;
        }
        case CppType::CPPTYPE_DOUBLE: {
          submodels_by_field_[field->number()] = submodels_by_row_[i] = new RepeatedDoubleModel(this, _protobuf, field);
          break;
        }
        case CppType::CPPTYPE_STRING: {
          submodels_by_field_[field->number()] = submodels_by_row_[i] = new RepeatedStringModel(this, _protobuf, field);
          break;
        }
      }
    } else if (field->cpp_type() == CppType::CPPTYPE_MESSAGE) {
      // Ignore all unset oneof fields if any is set
      if (IsCulledOneof_(refl, *_protobuf, field)) continue;
      // Only recursively build fields if they're set
      if (refl->HasField(*_protobuf, field)) {
        submodels_by_field_[field->number()] = submodels_by_row_[i] =
            new MessageModel(this, refl->MutableMessage(_protobuf, field));
      } else {
        submodels_by_field_[field->number()] = submodels_by_row_[i] = new MessageModel(this, field->message_type());
      }
    } else {
      submodels_by_field_[field->number()] = submodels_by_row_[i] = new PrimitiveModel(this, field);
    }
  }
}

int MessageModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  if (!_protobuf) return 0;
  return descriptor_->field_count();
}

int MessageModel::columnCount(const QModelIndex & /*parent*/) const { return 1; }

bool MessageModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  R_EXPECT(index.isValid(), false) << "Supplied index was invalid:" << index;

  const Descriptor *desc = _protobuf->GetDescriptor();
  const Reflection *refl = _protobuf->GetReflection();
  const FieldDescriptor *field = desc->field(index.row());
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

const ProtoModel *MessageModel::GetSubModel(const FieldPath &field_path) const {
  if (field_path.repeated_field_index != -1) {
    qDebug() << "Attempting to assign repeated index " << field_path.repeated_field_index << " of a non-repeated field";
    return nullptr;
  }
  if (!field_path) return this;
  auto smit = submodels_by_field_.find(field_path.front()->number());
  if (smit == submodels_by_field_.end()) return nullptr;
  return smit.value()->GetSubModel(field_path.SkipField());
}

QVariant MessageModel::Data() const {
  if (!_protobuf) return {};
  return QVariant::fromValue(AbstractMessage(*_protobuf));
}

bool MessageModel::SetData(const QVariant &value) {
  if (!value.canConvert<AbstractMessage>()) return false;
  AbstractMessage msg = value.value<AbstractMessage>();
  if (!msg) qDebug() << "Error: assigning null message to MessageModel";
  qDebug() << "Unimplemented: Assigning QVariant to MessageModel: " << msg->DebugString().c_str();
  return false;
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
    return submodels_by_row_[index.row()]->GetDisplayIcon();
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
    case CppType::CPPTYPE_BOOL: return refl->GetBool(*_protobuf, field);
    case CppType::CPPTYPE_ENUM: return refl->GetEnumValue(*_protobuf, field);
    case CppType::CPPTYPE_STRING: return QString::fromStdString(refl->GetString(*_protobuf, field));
  }

  return QVariant();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const {
  return dataInternal<true>(index, role);
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
        if (refType == type && model->DataAtRow(row).toString() == oldName) {
          model->SetDataAtRow(row, newName);
        }
      }
    }
  }
}
