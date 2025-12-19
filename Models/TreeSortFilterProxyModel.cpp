#include "TreeSortFilterProxyModel.h"
#include "ProtoModel.h"
#include "TreeModel.h"
#include "Object.pb.h"
#include "Sprite.pb.h"
#include "Sound.pb.h"
#include "Background.pb.h"
#include "Path.pb.h"
#include "Font.pb.h"
#include "Script.pb.h"
#include "Shader.pb.h"
#include "Timeline.pb.h"
#include "Room.pb.h"
#include "Settings.pb.h"
#include "treenode.pb.h"
#include <QDebug>
#include <Qt>

TreeSortFilterProxyModel::TreeSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {}


void TreeSortFilterProxyModel::SetFilterType(TreeNode::TypeCase type) {
  filterType = type;
  invalidateFilter();  // Force the filter to re-evaluate all rows
}

Qt::ItemFlags TreeSortFilterProxyModel::flags(const QModelIndex &index) const {
  Qt::ItemFlags baseFlags = QSortFilterProxyModel::flags(index);
  
  // Only modify flags if we have a filter type set
  if (filterType == TreeNode::TYPE_NOT_SET) return baseFlags;
  
  // Get the source index to check the actual node type
  QModelIndex sourceIndex = mapToSource(index);
  if (!sourceIndex.isValid()) return baseFlags;
  
  TreeModel::Node *node = static_cast<TreeModel::Node *>(sourceIndex.internalPointer());
  if (!node) return baseFlags;
  
  std::string msgType = node->GetMessageType();
  std::string treeNodeFullName(TreeNode::descriptor()->full_name());
  
  if (msgType == treeNodeFullName) {
    try {
      TreeNode item = node->GetMessage();
      TreeNode::TypeCase itemType = item.type_case();
      
      // Make folders non-selectable and non-enabled so they can't be selected
      // They'll still appear in the menu for navigation, but the action will be disabled
      if (itemType == TreeNode::kFolder && filterType != TreeNode::kFolder) {
        baseFlags &= ~Qt::ItemIsSelectable;
        baseFlags &= ~Qt::ItemIsEnabled;  // Disable folders so they can't be selected/triggered
      }
      // For all other types, keep the default flags (enabled and selectable)
    } catch (...) {
      // If GetMessage() fails, use default flags
    }
  }
  
  return baseFlags;
}

// Check if any child in the TreeModel matches the filter type
// This version takes a Node* directly to avoid model interface issues
bool hasMatchingChildNode(TreeModel::Node* node, TreeNode::TypeCase type) {
  if (!node) return false;
  
  std::string treeNodeFullName(TreeNode::descriptor()->full_name());
  
  // Check this node's children directly from the Node structure
  for (const auto& child : node->children) {
    if (!child) continue;
    
    // Try to get the TreeNode type_case
    std::string msgType = child->GetMessageType();
    TreeNode::TypeCase childType = TreeNode::TYPE_NOT_SET;
    
    if (msgType == treeNodeFullName) {
      // Child is a TreeNode - get its type_case directly
      try {
        TreeNode item = child->GetMessage();
        childType = item.type_case();
      } catch (...) {
        // If GetMessage() fails, skip this child
        continue;
      }
    } else {
      // Child is not a TreeNode - it's a resource message (Object, Sprite, etc.)
      // Map the resource message type to TreeNode::TypeCase
      if (msgType == "buffers.resources.Object") {
        childType = TreeNode::kObject;
      } else if (msgType == "buffers.resources.Sprite") {
        childType = TreeNode::kSprite;
      } else if (msgType == "buffers.resources.Sound") {
        childType = TreeNode::kSound;
      } else if (msgType == "buffers.resources.Background") {
        childType = TreeNode::kBackground;
      } else if (msgType == "buffers.resources.Path") {
        childType = TreeNode::kPath;
      } else if (msgType == "buffers.resources.Font") {
        childType = TreeNode::kFont;
      } else if (msgType == "buffers.resources.Script") {
        childType = TreeNode::kScript;
      } else if (msgType == "buffers.resources.Shader") {
        childType = TreeNode::kShader;
      } else if (msgType == "buffers.resources.Timeline") {
        childType = TreeNode::kTimeline;
      } else if (msgType == "buffers.resources.Room") {
        childType = TreeNode::kRoom;
      }
      // If we couldn't map it, childType remains TYPE_NOT_SET and we'll check children
    }
    
    // Check if the type matches
    if (childType == type) {
      return true;
    }
    
    // If it's a folder or any other type, check its children recursively
    if (hasMatchingChildNode(child.get(), type)) {
      return true;
    }
  }
  return false;
}

// Check if any child in the TreeModel matches the filter type
bool hasMatchingChild(const QAbstractItemModel* model, const QModelIndex& parent, TreeNode::TypeCase type) {
  // Get the Node directly from the parent index
  TreeModel::Node *parentNode = static_cast<TreeModel::Node *>(parent.internalPointer());
  if (parentNode) {
    // Use the Node's children directly
    return hasMatchingChildNode(parentNode, type);
  }
  
  // Fallback: use model interface if we can't get the node
  int rowCount = model->rowCount(parent);
  qDebug() << "hasMatchingChild: Fallback, checking" << rowCount << "children, filterType:" << (int)type;
  for (int row = 0; row < rowCount; ++row) {
    QModelIndex childIdx = model->index(row, 0, parent);
    if (!childIdx.isValid()) continue;
    
    TreeModel::Node *childNode = static_cast<TreeModel::Node *>(childIdx.internalPointer());
    if (!childNode) continue;
    
    // Try to get the TreeNode using GetMessage() which handles passthrough nodes
    std::string msgType = childNode->GetMessageType();
    if (msgType == TreeNode::descriptor()->full_name()) {
      TreeNode item = childNode->GetMessage();
      TreeNode::TypeCase itemType = item.type_case();
      if (itemType == type) return true;
      // If it's a folder or any other type, check its children recursively
      if (hasMatchingChild(model, childIdx, type)) return true;
    } else {
      // Not a TreeNode - check children recursively
      if (hasMatchingChild(model, childIdx, type)) return true;
    }
  }
  return false;
}

bool TreeSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {

  if (filterType == TreeNode::TYPE_NOT_SET) return true;

  QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);
  if (!idx.isValid()) {
    // Root node - accept it so we can traverse children
    return !sourceParent.isValid();
  }
  
  // TreeModel stores Node* in internalPointer, not TreeNode*
  TreeModel::Node *node = static_cast<TreeModel::Node *>(idx.internalPointer());
  if (!node) {
    // If no node pointer, might be root - accept it
    return !sourceParent.isValid();
  }
  
  // Check if this is a TreeNode message before trying to get it
  std::string msgType = node->GetMessageType();
  std::string treeNodeFullName(TreeNode::descriptor()->full_name());
  
  if (msgType != treeNodeFullName) {
    // Not a TreeNode - check if children match (might be a structural node)
    // Accept it if children match so we can traverse
    return hasMatchingChildNode(node, filterType);
  }
  
  // Use GetMessage() which handles passthrough nodes correctly
  TreeNode item = node->GetMessage();
  TreeNode::TypeCase itemType = item.type_case();
  
  // Check if the type matches exactly - this is what we want to show and make selectable
  if (itemType == filterType) {
    return true;
  }
  
  // For folders or other types, only accept if they contain matching children (for traversal)
  // But note: in QSortFilterProxyModel, accepting a folder makes it appear in the list
  // So we need to be careful - we only want to accept folders if they're needed for traversal
  // Actually, the issue is that if we accept a folder, it appears as a selectable item
  // But if we don't accept it, we can't see its children
  // The solution: only accept folders if they contain matching children, but the UI should
  // handle folders differently (e.g., make them non-selectable or expandable only)
  // For now, let's only accept folders if they contain matches, but they'll still appear
  // The real fix might need to be in the UI layer to make folders non-selectable
  if (itemType == TreeNode::kFolder) {
    // Only accept folder if it contains matching children
    return hasMatchingChildNode(node, filterType);
  }
  
  // For any other type that doesn't match, don't accept it
  return false;
}
