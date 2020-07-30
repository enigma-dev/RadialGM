#include "ProtoModel.h"

#include "Components/Logger.h"
#include "Components/ArtManager.h"

#include <QDataStream>
#include <QCoreApplication>

IconMap ProtoModel::iconMap;

ProtoModel::ProtoModel(QObject *parent, Message *protobuf) : QAbstractItemModel(parent), _protobuf(protobuf) {
  const Descriptor *desc = _protobuf->GetDescriptor();
  QSet<QString> uniqueMimes;
  QSet<const Descriptor*> visitedDesc;
  setupMimes(desc, uniqueMimes, visitedDesc);
  _mimes = uniqueMimes.values();
  qDebug() << _mimes;

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

void ProtoModel::setupMimes(const Descriptor* desc, QSet<QString>& uniqueMimes,
                            QSet<const Descriptor*>& visitedDesc) {
  if (!desc) return;
  visitedDesc.insert(desc);
  for (int i = 0; i < desc->field_count(); ++i) {
    const FieldDescriptor *field = desc->field(i);
    if (!field) continue;
    if (field->is_repeated()) {
      std::string typeName = "";
      if (field->type() == FieldDescriptor::TYPE_MESSAGE)
        typeName = field->message_type()->full_name();
      else
        typeName = field->type_name();
      //TODO: Add switch here to use real cross-application cross-process
      //mime type like e.g, application/string for the native field types
      uniqueMimes.insert(QString("RadialGM/") + QString::fromStdString(typeName));
    }
    if (field->type() == FieldDescriptor::TYPE_MESSAGE &&
        !visitedDesc.contains(field->message_type())) // << no infinite recursion
      setupMimes(field->message_type(), uniqueMimes, visitedDesc);
  }
}

QVariant ProtoModel::data(const QModelIndex &index, int role) const {
  R_EXPECT(index.isValid(), QVariant()) << "Supplied index was invalid:" << index;

  buffers::TreeNode *item = static_cast<buffers::TreeNode *>(index.internalPointer());
  if (role == Qt::DecorationRole) {
    auto it = iconMap.find(item->type_case());
    if (it == iconMap.end()) return ArtManager::GetIcon("info");

    if (item->type_case() == TypeCase::kSprite) {
      if (item->sprite().subimages_size() <= 0) return QVariant();
      QString spr = QString::fromStdString(item->sprite().subimages(0));
      return spr.isEmpty() ? QVariant() : ArtManager::GetIcon(spr);
    }

    if (item->type_case() == TypeCase::kBackground) {
      QString bkg = QString::fromStdString(item->background().image());
      return bkg.isEmpty() ? QVariant() : ArtManager::GetIcon(bkg);
    }

    const QIcon &icon = it->second;
    if (item->type_case() == TypeCase::kFolder && item->child_size() <= 0) {
      return QIcon(icon.pixmap(icon.availableSizes().first(), QIcon::Disabled));
    }
    return icon;
  } else if (role == Qt::EditRole || role == Qt::DisplayRole) {
    return QString::fromStdString(item->name());
  }

  return QVariant();
}

bool ProtoModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  R_EXPECT(index.isValid(), false) << "Supplied index was invalid:" << index;
  if (role != Qt::EditRole) return false;

  buffers::TreeNode *item = static_cast<buffers::TreeNode *>(index.internalPointer());
  const QString oldName = QString::fromStdString(item->name());
  const QString newName = value.toString();
  if (oldName == newName) return true;
  item->set_name(newName.toStdString());
  emit ResourceRenamed(item->type_case(), oldName, value.toString());
  emit dataChanged(index, index);
  return true;
}

void ProtoModel::SetDirty(bool dirty) { _dirty = dirty; }

bool ProtoModel::IsDirty() { return _dirty; }

int ProtoModel::columnCount(const QModelIndex &parent) const {
  Message *parentItem;
  if (!parent.isValid())
    parentItem = _protobuf;
  else
    parentItem = static_cast<Message *>(parent.internalPointer());

  const Descriptor *desc = parentItem->GetDescriptor();
  return desc->field_count();
}

QModelIndex ProtoModel::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent)) return QModelIndex();

  Message *parentItem;
  if (!parent.isValid())
    parentItem = _protobuf;
  else
    parentItem = static_cast<Message *>(parent.internalPointer());

  const Descriptor *desc = parentItem->GetDescriptor();
  const Reflection *refl = parentItem->GetReflection();
  const FieldDescriptor *field = desc->FindFieldByNumber(parent.column());
  Message *childItem = refl->MutableRepeatedMessage(parentItem, field, parent.row());
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex ProtoModel::parent(const QModelIndex &index) const {
  if (!index.isValid()) return QModelIndex();
  return parents[index];
}

Qt::ItemFlags ProtoModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return nullptr;
  auto flags = QAbstractItemModel::flags(index);
  if (canSetData(index))
    flags |= Qt::ItemIsEditable;
  // TODO: check if parent index is repeated
  flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
  return flags;
}

Qt::DropActions ProtoModel::supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

QStringList ProtoModel::mimeTypes() const { return _mimes; }

QMimeData *ProtoModel::mimeData(const QModelIndexList &indexes) const {
  QMimeData *mimeData = new QMimeData();

  QList<QModelIndex> nodes;
  for (const QModelIndex &index : indexes) {
    if (!index.isValid() || nodes.contains(index)) continue;
    nodes << index;
  }

  // rows are moved starting with the lowest so we can create
  // unique names in the order of insertion
  std::sort(nodes.begin(), nodes.end(), std::less<QModelIndex>());

  //TODO: Handle non-message mimes here like string & int
  //ideally pass index to data() and check type of returned
  //variant to get the correct mime type
  QHash<QString,QList<QModelIndex>> indexesByMime;
  for (const QModelIndex &index : nodes) {
    Message *msg = static_cast<Message *>(index.internalPointer());
    std::string typeName = msg->GetTypeName();
    QString mime = "RadialGM/" + QString::fromStdString(typeName);
    indexesByMime[mime].append(index);
  }

  for (auto it = indexesByMime.begin(); it != indexesByMime.end(); ++it) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << QCoreApplication::applicationPid();
    stream << nodes.count();
    for (const QModelIndex &index : it.value()) {
      Message *node = static_cast<Message *>(index.internalPointer());
      stream << reinterpret_cast<qlonglong>(node) << index.row();
    }
    mimeData->setData(it.key(), data);
  }

  return mimeData;
}

bool ProtoModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int /*column*/,
                             const QModelIndex &parent) {
  return false;
}
