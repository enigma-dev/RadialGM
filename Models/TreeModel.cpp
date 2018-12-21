#include "TreeModel.h"

#include "Components/ArtManager.h"
#include "Models/ResourceModelMap.h"

#include <QCoreApplication>
#include <QMimeData>

IconMap TreeModel::iconMap;

TreeModel::TreeModel(buffers::TreeNode *root, ResourceModelMap *resourceMap, QObject *parent)
    : QAbstractItemModel(parent), root(root), resourceMap(resourceMap) {
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

  SetupParents(root);
}

void TreeModel::SetupParents(buffers::TreeNode *root) {
  for (int i = 0; i < root->child_size(); ++i) {
    auto child = root->mutable_child(i);
    parents[child] = root;
    SetupParents(child);
  }
}

int TreeModel::columnCount(const QModelIndex & /*parent*/) const { return 1; }

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (!index.isValid() || role != Qt::EditRole) return false;
  buffers::TreeNode *item = static_cast<buffers::TreeNode *>(index.internalPointer());
  const QString oldName = QString::fromStdString(item->name());
  const QString newName = value.toString();
  if (oldName == newName) return true;
  item->set_name(newName.toStdString());
  emit ResourceRenamed(item->type_case(), oldName, value.toString());
  emit dataChanged(index, index);
  return true;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return QVariant();

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

    if (item->type_case() == TypeCase::kObject) {
      const ProtoModel *sprModel = GetObjectSprite(item->name());
      if (sprModel == nullptr) return QVariant();
      QString spr = sprModel->GetString(Sprite::kSubimagesFieldNumber, 0);
      return spr.isEmpty() ? QVariant() : ArtManager::GetIcon(spr);
    }

    const QIcon &icon = it->second;
    if (item->type_case() == TypeCase::kFolder && item->child_size() <= 0) {
      return icon.pixmap(icon.availableSizes().first(), QIcon::Disabled);
    }
    return icon;
  } else if (role == Qt::EditRole || role == Qt::DisplayRole) {
    return QString::fromStdString(item->name());
  }

  return QVariant();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);

  if (index.isValid()) {
    auto *node = static_cast<TreeNode *>(index.internalPointer());
    if (node->folder()) flags |= Qt::ItemIsDropEnabled;
    return Qt::ItemIsDragEnabled | Qt::ItemIsEditable | flags;
  } else
    return Qt::ItemIsDropEnabled | flags;
}

QVariant TreeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  return tr("Name");
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent)) return QModelIndex();

  buffers::TreeNode *parentItem;

  if (!parent.isValid())
    parentItem = root;
  else
    parentItem = static_cast<buffers::TreeNode *>(parent.internalPointer());

  buffers::TreeNode *childItem = parentItem->mutable_child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
  if (!index.isValid()) return QModelIndex();

  buffers::TreeNode *childItem = static_cast<buffers::TreeNode *>(index.internalPointer());
  buffers::TreeNode *parentItem = parents[childItem];

  if (parentItem == root || !parentItem) return QModelIndex();

  return createIndex(parentItem->child_size(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const {
  buffers::TreeNode *parentItem;
  if (parent.column() > 0) return 0;

  if (!parent.isValid())
    parentItem = root;
  else
    parentItem = static_cast<buffers::TreeNode *>(parent.internalPointer());

  return parentItem->child_size();
}

Qt::DropActions TreeModel::supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

QStringList TreeModel::mimeTypes() const { return QStringList(treeNodeMime()); }

QMimeData *TreeModel::mimeData(const QModelIndexList &indexes) const {
  QMimeData *mimeData = new QMimeData();
  QByteArray data;

  QDataStream stream(&data, QIODevice::WriteOnly);
  QList<QModelIndex> nodes;

  for (const QModelIndex &index : indexes) {
    if (!index.isValid() || nodes.contains(index)) continue;
    nodes << index;
  }

  // rows are moved starting with the lowest so we can create
  // unique names in the order of insertion
  std::sort(nodes.begin(), nodes.end(), std::less<QModelIndex>());

  stream << QCoreApplication::applicationPid();
  stream << nodes.count();
  for (const QModelIndex &index : nodes) {
    TreeNode *node = static_cast<TreeNode *>(index.internalPointer());
    stream << reinterpret_cast<qlonglong>(node) << index.row();
  }
  mimeData->setData(treeNodeMime(), data);
  return mimeData;
}

bool TreeModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int /*column*/,
                             const QModelIndex &parent) {
  if (action != Qt::MoveAction && action != Qt::CopyAction) return false;
  // ensure the data is in the format we expect
  if (!mimeData->hasFormat(treeNodeMime())) return false;
  QByteArray data = mimeData->data(treeNodeMime());
  QDataStream stream(&data, QIODevice::ReadOnly);

  qint64 senderPid;
  stream >> senderPid;
  // ensure the data is coming from the same process since mime is pointer based
  if (senderPid != QCoreApplication::applicationPid()) return false;

  TreeNode *parentNode = static_cast<TreeNode *>(parent.internalPointer());
  if (!parentNode) parentNode = root;
  int count;
  stream >> count;
  if (count <= 0) return false;
  if (row == -1) row = rowCount(parent);
  QHash<TreeNode *, unsigned> removedCount;

  for (int i = 0; i < count; ++i) {
    qlonglong nodePtr;
    stream >> nodePtr;
    int itemRow;
    stream >> itemRow;
    TreeNode *node = reinterpret_cast<TreeNode *>(nodePtr);

    if (action != Qt::CopyAction) {
      auto *oldParent = parents[node];

      // offset the row we are removing by the number of
      // rows already removed from the same parent
      if (parentNode != oldParent || row > itemRow) {
        itemRow -= removedCount[oldParent]++;
      }

      // if moving the node within the same parent we need to adjust the row
      // since its own removal will affect the row we reinsert it at
      if (parentNode == oldParent && row > itemRow) --row;

      auto index = this->createIndex(itemRow, 0, node);
      beginRemoveRows(index.parent(), itemRow, itemRow);
      auto oldRepeated = oldParent->mutable_child();
      oldRepeated->ExtractSubrange(itemRow, 1, nullptr);
      parents.remove(node);
      endRemoveRows();
    } else {
      // duplicate the node
      auto *dup = node->New();
      dup->CopyFrom(*node);
      node = dup;
      // give the duplicate node a new name
      const QString name = resourceMap->CreateResourceName(node);
      node->set_name(name.toStdString());
      // add the new node to the resource map
      resourceMap->AddResource(node, resourceMap);
    }

    beginInsertRows(parent, row, row);
    parentNode->mutable_child()->AddAllocated(node);
    for (int j = parentNode->child_size() - 1; j > row; --j) {
      parentNode->mutable_child()->SwapElements(j, j - 1);
    }
    parents[node] = parentNode;
    endInsertRows();

    ++row;
  }

  return true;
}

void TreeModel::addNode(buffers::TreeNode *child, buffers::TreeNode *parent) {
  auto rootIndex = QModelIndex();
  emit beginInsertRows(rootIndex, parent->child_size(), parent->child_size());
  parent->mutable_child()->AddAllocated(child);
  parents[child] = parent;
  emit endInsertRows();
}

void TreeModel::removeNode(const QModelIndex &childIndex) {
  if (!childIndex.isValid()) return;
  auto *child = static_cast<buffers::TreeNode *>(childIndex.internalPointer());
  buffers::TreeNode *parent = parents[child];
  int pos = 0;
  for (; pos < parent->child_size(); ++pos)
    if (parent->mutable_child(pos) == child) break;
  emit beginRemoveRows(childIndex.parent(), pos, pos);
  parent->mutable_child()->DeleteSubrange(pos, 1);
  emit endRemoveRows();
}
