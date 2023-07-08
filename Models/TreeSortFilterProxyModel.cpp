#include "TreeSortFilterProxyModel.h"
#include "ProtoModel.h"

TreeSortFilterProxyModel::TreeSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {}


void TreeSortFilterProxyModel::SetFilterType(TreeNode::TypeCase type) {
  filterType = type;
}

inline bool recHasType(TreeNode& n, TreeNode::TypeCase type) {
  if (!n.has_folder()) return false;
  for (auto child : n.folder().children()) {
    if (child.type_case() == TreeNode::kFolder) return recHasType(child, type);
    if (child.type_case() == type) return true;
  }
  return false;
}

bool TreeSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {

  if (filterType == TreeNode::TYPE_NOT_SET) return true;

  QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);
  buffers::TreeNode *item = static_cast<buffers::TreeNode *>(idx.internalPointer());

  if (item->has_folder() && item->folder().children_size() > 0) {
    return recHasType(*item, filterType);
  }

  return item->type_case() == filterType;
}
