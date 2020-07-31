#include "TreeModel.h"

#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "Models/RepeatedImageModel.h"
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

QVariant TreeModel::data(const QModelIndex &index, int role) const {
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

    if (item->type_case() == TypeCase::kObject) {
      MessageModel *sprModel = GetObjectSprite(item->name());
      if (sprModel == nullptr) return QVariant();
      if (!sprModel->GetSubModel<RepeatedImageModel *>(Sprite::kSubimagesFieldNumber)->Empty()) {
        QString spr = sprModel->GetSubModel<RepeatedImageModel *>(Sprite::kSubimagesFieldNumber)->Data(0).toString();
        return spr.isEmpty() ? QVariant() : ArtManager::GetIcon(spr);
      }
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

  buffers::TreeNode *parentParentItem = parents[parentItem];
  if (!parentParentItem) parentParentItem = root;

  // get the row number for the parent from its own parent
  int pos = 0;
  for (; pos < parentParentItem->child_size(); ++pos)
    if (parentParentItem->mutable_child(pos) == parentItem) break;
  if (pos == parentParentItem->child_size()) return QModelIndex();
  return createIndex(pos, 0, parentItem);
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
  using pathComparator = std::function<bool(const QModelIndex & a, const QModelIndex & b)>;
  pathComparator compareIndexes =
    [&compareIndexes](const QModelIndex & a, const QModelIndex & b) -> bool {
      return a < b && compareIndexes(a.parent(),b.parent());
    };
  std::sort(nodes.begin(), nodes.end(), compareIndexes);

  stream << QCoreApplication::applicationPid();
  stream << nodes.count();
  for (const QModelIndex &index : nodes) {
    TreeNode *node = static_cast<TreeNode *>(index.internalPointer());
    stream << reinterpret_cast<qlonglong>(node) << index.row();
  }
  mimeData->setData(treeNodeMime(), data);
  return mimeData;
}

template <typename T>
static void RepeatedFieldInsert(RepeatedPtrField<T> *field, T *newItem, int index) {
  field->AddAllocated(newItem);
  for (int j = field->size() - 1; j > index; --j) {
    field->SwapElements(j, j - 1);
  }
}

QModelIndex TreeModel::insert(const QModelIndex &parent, int row, buffers::TreeNode *node) {
  auto insertIndex = parent;
  if (!parent.isValid()) insertIndex = QModelIndex();
  auto *parentNode = static_cast<buffers::TreeNode *>(insertIndex.internalPointer());
  if (!parentNode) {
    insertIndex = QModelIndex();
    parentNode = root;
  }
  beginInsertRows(insertIndex, row, row);
  RepeatedFieldInsert<buffers::TreeNode>(parentNode->mutable_child(), node, row);
  parents[node] = parentNode;
  endInsertRows();

  return this->index(row, 0, insertIndex);
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
        itemRow -= removedCount[oldParent];
      }

      auto index = this->createIndex(itemRow, 0, node);
      bool canDo = beginMoveRows(index.parent(), itemRow, itemRow, parent, row);
      if (!canDo) continue;

      // count this row as having been moved from this parent
      if (parentNode != oldParent || row > itemRow) removedCount[oldParent]++;

      // if moving the node within the same parent we need to adjust the row
      // since its own removal will affect the row we reinsert it at
      if (parentNode == oldParent && row > itemRow) --row;

      auto oldRepeated = oldParent->mutable_child();
      oldRepeated->ExtractSubrange(itemRow, 1, nullptr);
      RepeatedFieldInsert<buffers::TreeNode>(parentNode->mutable_child(), node, row);
      parents[node] = parentNode;
      endMoveRows();
      ++row;
    } else {
      if (node->folder()) continue;
      node = duplicateNode(*node);
      insert(parent, row++, node);
    }
  }

  return true;
}

QModelIndex TreeModel::addNode(buffers::TreeNode *child, const QModelIndex &parent) {
  auto insertParent = parent;
  int pos = 0;
  if (parent.isValid()) {
    buffers::TreeNode *parentNode = static_cast<buffers::TreeNode *>(parent.internalPointer());
    if (parentNode->has_folder()) {
      pos = parentNode->child_size();
    } else {
      insertParent = parent.parent();
      parentNode = static_cast<buffers::TreeNode *>(insertParent.internalPointer());
      if (!parentNode) {
        parentNode = root;
      }
      pos = parent.row();
    }
  } else {
    insertParent = QModelIndex();
    pos = this->root->child_size();
  }

  return insert(insertParent, pos, child);
}

buffers::TreeNode *TreeModel::duplicateNode(const buffers::TreeNode &node) {
  // duplicate the node
  auto *dup = node.New();
  dup->CopyFrom(node);
  // give the duplicate node a new name
  const QString name = resourceMap->CreateResourceName(dup);
  dup->set_name(name.toStdString());
  // add the new node to the resource map
  resourceMap->AddResource(dup);
  return dup;
}

void TreeModel::removeNode(const QModelIndex &index) {
  if (!index.isValid()) return;
  auto *node = static_cast<buffers::TreeNode *>(index.internalPointer());
  if (!node) return;
  if (node->has_folder()) {
    for (int i = node->child_size(); i > 0; --i) {
      removeNode(this->index(i - 1, 0, index));
    }
  }
  buffers::TreeNode *parent = parents[node];
  int pos = 0;
  for (; pos < parent->child_size(); ++pos)
    if (parent->mutable_child(pos) == node) break;
  if (pos == parent->child_size()) return;  // already removed?
  emit beginRemoveRows(index.parent(), pos, pos);
  resourceMap->RemoveResource(node->type_case(), QString::fromStdString(node->name()));
  parent->mutable_child()->DeleteSubrange(pos, 1);
  emit endRemoveRows();
}

void TreeModel::sortByName(const QModelIndex &index) {
  if (!index.isValid()) return;
  auto *node = static_cast<buffers::TreeNode *>(index.internalPointer());
  if (!node) return;
  auto *child_field = node->mutable_child();
  std::sort(child_field->begin(), child_field->end(),
            [](const buffers::TreeNode &a, const buffers::TreeNode &b) { return a.name() < b.name(); });
  emit dataChanged(this->index(0, 0, index), this->index(node->child_size(), 0, index));
}
