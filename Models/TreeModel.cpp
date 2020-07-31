#include "TreeModel.h"

#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "Models/ResourceModelMap.h"

#include <QCoreApplication>
#include <QMimeData>

TreeModel::TreeModel(buffers::TreeNode *root, ResourceModelMap *resourceMap, QObject *parent)
    : ProtoModel(parent, root), root(root), resourceMap(resourceMap) {}

QVariant TreeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  return tr("Name");
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
  //qDebug() << "index" << hasIndex(0,0,QModelIndex()) << row;
  if (!hasIndex(row, column, parent)) return QModelIndex();

  buffers::TreeNode *parentItem;

  if (!parent.isValid())
    parentItem = root;
  else
    parentItem = static_cast<buffers::TreeNode *>(parent.internalPointer());

  buffers::TreeNode *childItem = parentItem->mutable_child(row);
  if (childItem) {
    QModelIndex ret = createIndex(row, column, childItem);
    const_cast<TreeModel*>(this)->parents[ret] = parent;
    return ret;
  } else
    return QModelIndex();
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
  parents[index(row,0,parent)] = parent;
  endInsertRows();

  return this->index(row, 0, insertIndex);
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
