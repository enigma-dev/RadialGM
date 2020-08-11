#include "ProtoModel.h"

#include "Components/ArtManager.h"

#include "options.pb.h"

#include <google/protobuf/reflection.h>

#include <QDataStream>
#include <QCoreApplication>

using namespace buffers;

using CppType = FieldDescriptor::CppType;

IconMap ProtoModel::iconMap;

ProtoModel::ProtoModel(QObject *parent, Message *protobuf) : QAbstractItemModel(parent), _protobuf(protobuf) {
  const Descriptor *desc = _protobuf->GetDescriptor();

  iconMap = {{TypeCase::kFolder, ArtManager::GetIcon("group")},
             {TypeCase::kSprite, ArtManager::GetIcon("sprite")},
             {TypeCase::kSound, ArtManager::GetIcon("sound")},
             {TypeCase::kBackground, ArtManager::GetIcon("background")},
             {TypeCase::kPath, ArtManager::GetIcon("path")},
             {TypeCase::kScript, ArtManager::GetIcon("script")},
             {TypeCase::kShader, ArtManager::GetIcon("shader")},
             {TypeCase::kFont, ArtManager::GetIcon("font")},
             {TypeCase::kTimeline, ArtManager::GetIcon("timeline")},
             {TypeCase::kObject, ArtManager::GetIcon("object")},
             {TypeCase::kRoom, ArtManager::GetIcon("room")},
             {TypeCase::kSettings, ArtManager::GetIcon("settings")}};
}

QMap<int, QVariant> ProtoModel::itemData(const QModelIndex &index) const {
  QMap<int, QVariant> roles;
  if (IsMessage(index)) {
    // grab a pointer for messages
    QVariant variantData = data(index, Qt::UserRole+1);
    if (variantData.isValid())
        roles.insert(Qt::UserRole+1, variantData);
  } else {
    QAbstractItemModel::itemData(index);
  }

  return roles;
}

QVariant ProtoModel::data(const QModelIndex &index, int role) const {
  if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::DecorationRole &&
      role != Qt::UserRole+1) return QVariant();
  auto message = GetMessage(index);

  if (IsMessage(index)) {
    // mutable pointer to the message requested for editing
    if (role == Qt::UserRole+1) return QVariant::fromValue((quintptr)message);

    // let's be nice and automagically handle tree nodes
    // for some simple convenience
    if (message->GetTypeName() == "buffers.TreeNode") {
      buffers::TreeNode *item = static_cast<buffers::TreeNode *>(message);
      if (role == Qt::DecorationRole) {
        auto it = iconMap.find(item->type_case());
        if (it == iconMap.end()) return ArtManager::GetIcon("info");

        const QIcon &icon = it->second;
        if (item->type_case() == TypeCase::kFolder && item->child_size() <= 0) {
          return QIcon(icon.pixmap(icon.availableSizes().first(), QIcon::Disabled));
        }
        return icon;
      } else if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return QString::fromStdString(item->name());
      }
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole)
      return QString::fromStdString(message->GetTypeName());

    return QVariant();
  }

  if (role != Qt::DisplayRole && role != Qt::EditRole &&
      role != Qt::DecorationRole) return QVariant();
  auto desc = message->GetDescriptor();
  auto refl = message->GetReflection();
  auto field = desc->field(index.row());
  R_EXPECT(field != nullptr, QVariant()) << "Reading non-existant field index" << index.row()
                                         << "for message" << message << "with type"
                                         << QString::fromStdString(message->GetTypeName());

  if (field->is_repeated() && role != Qt::DecorationRole)
    return QString::fromStdString(field->name());

  if (role == Qt::DecorationRole) {
    auto options = field->options();
    if (options.HasExtension(buffers::file_kind)) {
      auto filekind = options.GetExtension(buffers::file_kind);
      if (filekind == buffers::FileKind::IMAGE) {
        auto filepath = refl->GetString(*message, field);
        return QPixmap(QString::fromStdString(filepath));
      }
    }
    return QVariant();
  }

  switch (field->cpp_type()) {
    case CppType::CPPTYPE_MESSAGE:
      return QString::fromStdString(field->name());
    case CppType::CPPTYPE_INT32:
      return refl->GetInt32(*message, field);
    case CppType::CPPTYPE_INT64:
      return static_cast<long long>(refl->GetInt64(*message, field));
    case CppType::CPPTYPE_UINT32:
      return refl->GetUInt32(*message, field);
    case CppType::CPPTYPE_UINT64:
      return static_cast<unsigned long long>(refl->GetUInt64(*message, field));
    case CppType::CPPTYPE_DOUBLE:
      return refl->GetDouble(*message, field);
    case CppType::CPPTYPE_FLOAT:
      return refl->GetFloat(*message, field);
    case CppType::CPPTYPE_BOOL:
      return refl->GetBool(*message, field);
    case CppType::CPPTYPE_ENUM:
      return refl->GetEnumValue(*message, field);
    case CppType::CPPTYPE_STRING:
      return QString::fromStdString(refl->GetString(*message, field));
  }

  return QVariant();
}

bool ProtoModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (role == Qt::UserRole) return true; // << was an editable test for flags
  if (role != Qt::EditRole && role != Qt::UserRole+1) return false;
  if (IsMessage(index)) {
    //TODO: why the fuck am i handling tree node bullshit here?
    auto message = GetMessage(index);
    if (role == Qt::EditRole && message->GetTypeName() == "buffers.TreeNode") {
      auto treeNode = static_cast<TreeNode*>(message);
      treeNode->set_name(value.toString().toStdString());
      emit dataChanged(index, index);
      return true;
    }
    //TODO: Add Qt::UserRole+2 for IPC serialization
    //TODO: Add enum for all these user roles
    if (role != Qt::UserRole+1) return false;
    auto submessage = reinterpret_cast<Message*>(value.value<quintptr>());
    message->CopyFrom(*submessage);
    emit dataChanged(index, index);
    return true;
  }
  auto message = GetMessage(index);
  auto desc = message->GetDescriptor();
  auto refl = message->GetReflection();
  auto field = desc->field(index.row());
  R_EXPECT(field != nullptr, false) << "Writing non-existant field index" << index.row()
                                    << "for message" << message << "with type"
                                    << QString::fromStdString(message->GetTypeName());

  switch (field->cpp_type()) {
    case CppType::CPPTYPE_MESSAGE: {
      if (role != Qt::UserRole+1) return false;
      auto submessage = reinterpret_cast<Message*>(value.value<quintptr>());
      refl->SetAllocatedMessage(message, submessage, field);
      break;
    }
    case CppType::CPPTYPE_INT32: refl->SetInt32(message, field, value.toInt()); break;
    case CppType::CPPTYPE_INT64: refl->SetInt64(message, field, value.toLongLong()); break;
    case CppType::CPPTYPE_UINT32: refl->SetUInt32(message, field, value.toUInt()); break;
    case CppType::CPPTYPE_UINT64: refl->SetUInt64(message, field, value.toULongLong()); break;
    case CppType::CPPTYPE_DOUBLE: refl->SetDouble(message, field, value.toDouble()); break;
    case CppType::CPPTYPE_FLOAT: refl->SetFloat(message, field, value.toFloat()); break;
    case CppType::CPPTYPE_BOOL: refl->SetBool(message, field, value.toBool()); break;
    case CppType::CPPTYPE_ENUM:
      refl->SetEnum(message, field, field->enum_type()->FindValueByNumber(value.toInt()));
      break;
    case CppType::CPPTYPE_STRING:
      refl->SetString(message, field, value.toString().toStdString());
      break;
  }

  //TODO: FIXME
  //emit ResourceRenamed(item->type_case(), oldName, value.toString());
  emit dataChanged(index, index);
  return true;
}

int ProtoModel::rowCount(const QModelIndex &parent) const {
  auto message = GetMessage(parent);
  auto desc = message->GetDescriptor();

  // message children are always fields
  // that includes the root message
  if (IsMessage(parent)) return desc->field_count();

  auto field = desc->field(parent.row());
  if (field->is_repeated()) {
    auto refl = message->GetReflection();
    return refl->FieldSize(*message,field);
  }
  if (field->cpp_type() == CppType::CPPTYPE_MESSAGE) {
    auto refl = message->GetReflection();
    return refl->HasField(*message,field);
  }
  //TODO: Handle enum?
  return 0;
}

int ProtoModel::columnCount(const QModelIndex&) const {
  return 1;
}

QModelIndex ProtoModel::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent)) return QModelIndex();

  Message *parentItem = GetMessage(parent);
  // if parent is a message, we are a field
  if (IsMessage(parent)) {
    parentItem = field_pointer(parentItem, row);
  } else {
    auto desc = parentItem->GetDescriptor();
    auto refl = parentItem->GetReflection();
    auto field = desc->field(parent.row());
    if (field->cpp_type() == CppType::CPPTYPE_MESSAGE)
      parentItem = field->is_repeated() ?
            refl->MutableRepeatedMessage(parentItem,field,row) :
            refl->MutableMessage(parentItem,field);
  }

  auto index = createIndex(row, column, parentItem);
  auto& mutableParents = const_cast<ProtoModel*>(this)->parents;
  if (index == parent) {
    TreeNode *msg = nullptr;
    msg->name();
  }
  R_EXPECT(index != parent, QModelIndex()) << "Model index equal to parent: " << index;
  mutableParents[index] = parent;
  return index;
}

QModelIndex ProtoModel::parent(const QModelIndex &index) const {
  if (!index.isValid()) return QModelIndex();
  auto it = parents.find(index);
  if (it == parents.end()) return QModelIndex();
  return it.value();
}

static void RepeatedFieldAdd(Message* message, const Reflection* refl,
                             const FieldDescriptor* field) {
  switch (field->cpp_type()) {
    case CppType::CPPTYPE_MESSAGE:
      refl->AddMessage(message,field); break;
    case CppType::CPPTYPE_INT32:
      refl->GetMutableRepeatedFieldRef<int32_t>(message,field).Add(
            field->default_value_int32());
      break;
    case CppType::CPPTYPE_INT64:
      refl->GetMutableRepeatedFieldRef<int64_t>(message,field).Add(
            field->default_value_int64());
      break;
    case CppType::CPPTYPE_UINT32:
      refl->GetMutableRepeatedFieldRef<uint32_t>(message,field).Add(
            field->default_value_uint32());
      break;
    case CppType::CPPTYPE_UINT64:
      refl->GetMutableRepeatedFieldRef<uint64_t>(message,field).Add(
            field->default_value_uint64());
      break;
    case CppType::CPPTYPE_DOUBLE:
      refl->GetMutableRepeatedFieldRef<double>(message,field).Add(
            field->default_value_double());
      break;
    case CppType::CPPTYPE_FLOAT:
      refl->GetMutableRepeatedFieldRef<float>(message,field).Add(
            field->default_value_float());
      break;
    case CppType::CPPTYPE_BOOL:
      refl->GetMutableRepeatedFieldRef<bool>(message,field).Add(
          field->default_value_bool());
      break;
    case CppType::CPPTYPE_ENUM:
      refl->GetMutableRepeatedFieldRef<int32_t>(message,field).Add(
            field->default_value_enum()->number());
      break;
    case CppType::CPPTYPE_STRING:
      refl->GetMutableRepeatedFieldRef<std::string>(message,field).Add(
            field->default_value_string());
      break;
  }
}

bool ProtoModel::RepeatedMutateSetup(Message** message, const FieldDescriptor** field,
                                     const QModelIndex& parent) const {
  R_EXPECT(IsField(parent),false)
      << "Repeated mutate of non-field index:" << parent;
  *message = GetMessage(parent);
  auto desc = (*message)->GetDescriptor();
  *field = desc->field(parent.row());
  R_EXPECT((*field)->is_repeated(),false)
      << "Repeated mutate of non-repeated field:" << parent;
  return true;
}

//TODO: Fix this quadratic behavior, ask Josh how to rotate.
//And how to rotate it in block + count too for efficiency.
static void RepeatedFieldMove(Message* message, const Reflection* refl,
                             const FieldDescriptor* field, int index1, int index2) {
  if (index1 < index2)
    for (int j = index1; j < index2; ++j)
      refl->SwapElements(message, field, j, j + 1);
  else
    for (int j = index1; j > index2; --j)
      refl->SwapElements(message, field, j, j - 1);
}

bool ProtoModel::insertRows(int row, int count, const QModelIndex &parent) {
  Message* message; const FieldDescriptor* field;
  if (!RepeatedMutateSetup(&message, &field, parent))
    return false;
  auto refl = message->GetReflection();

  beginInsertRows(parent, row, row+count-1);
  //TODO: Fix quadratic behavior below, ask Josh how to rotate.
  for (int i = 0; i < count; ++i) {
    RepeatedFieldAdd(message, refl, field);
    RepeatedFieldMove(message, refl, field, refl->FieldSize(*message,field)-1, row+i);
  }
  endInsertRows();
  return true;
}

bool ProtoModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                          const QModelIndex &destinationParent, int destinationChild) {
  Message *sourceMessage, *destMessage; const FieldDescriptor *sourceField, *destField;
  if (!RepeatedMutateSetup(&sourceMessage, &sourceField, sourceParent) ||
      !RepeatedMutateSetup(&destMessage, &destField, destinationParent))
    return false;
  R_EXPECT(sourceField->cpp_type() == destField->cpp_type(), false)
      << "Move repeated fields with incompatible types:"
      << "from" << sourceParent << "to" << destinationParent
      << "with types" << sourceField->cpp_type() << "&" << destField->cpp_type();
  auto sourceRefl = sourceMessage->GetReflection();
  auto destRefl = destMessage->GetReflection();

  int destinationDelta = 0;
  if (sourceParent == destinationParent && sourceRow < destinationChild)
    destinationDelta = count;
  if (!beginMoveRows(sourceParent, sourceRow,
                     sourceRow + count-1, destinationParent,
                     destinationChild + destinationDelta))
    return false;

  if (sourceParent == destinationParent) {
    for (int i = 0; i < count; ++i)
      RepeatedFieldMove(destMessage, destRefl, destField, sourceRow, destinationChild+i);
  } else {

  }

  endMoveRows();
  return true;
}

bool ProtoModel::removeRows(int row, int count, const QModelIndex &parent) {
  Message* message; const FieldDescriptor* field;
  if (!RepeatedMutateSetup(&message, &field, parent))
    return false;
  auto refl = message->GetReflection();

  beginRemoveRows(parent, row, row+count-1);
  //TODO: Fix quadratic behavior below, ask Josh how to rotate.
  for (int i = 0; i < count; ++i) {
    // Remove the elements from last to first to keep correct rows.
    RepeatedFieldMove(message, refl, field, row+count-1-i, refl->FieldSize(*message,field)-1);
    refl->RemoveLast(message,field);
  }
  endRemoveRows();
  return true;
}

Qt::ItemFlags ProtoModel::flags(const QModelIndex &index) const {
  // invalid indexes are accepted here because they correspond
  // to the root and allow dropping between children of the root
  auto flags = QAbstractItemModel::flags(index);
  if (canSetData(index))
    flags |= Qt::ItemIsEditable;
  // TODO: check if parent index is repeated
  flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
  return flags;
}

Qt::DropActions ProtoModel::supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

QStringList ProtoModel::mimeTypes() const { return {"RadialGM/ProtoModel"}; }

bool ProtoModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row,
                                 int, const QModelIndex &parent) const {
  if (action != Qt::MoveAction && action != Qt::CopyAction) return false;
  const bool supportsProto = data->hasFormat("RadialGM/ProtoModel");
  //qDebug() << row << parent;

  if (IsMessage(parent)) {
    // messages do not support dropping "between" their fields
    // you can only drop on to a message index or on to and
    // into one of their fields
    if (row != -1) return false;
    if (!supportsProto) return false;
    // now peek at the proto index being dropped and check compatibility

    return true;
  }
  // this is a field so look up its parent message
  auto message = GetMessage(parent);
  auto desc = message->GetDescriptor();
  auto field = desc->field(parent.row());
  // you can only drop on to a non-repeated field not
  // into one, this only occurs with non-repeated
  // message fields which have a single child, the message
  if (row != -1 && !field->is_repeated()) return false;

  //TODO: finish

  return true;
}
