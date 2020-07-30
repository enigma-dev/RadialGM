#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "ResourceModelMap.h"
#include "ProtoModel.h"
#include "Components/ArtManager.h"
#include "treenode.pb.h"

#include <QHash>

#include <unordered_map>

using TypeCase = buffers::TreeNode::TypeCase;

class TreeModel : public ProtoModel {
  Q_OBJECT

 public:
  explicit TreeModel(buffers::TreeNode *root, ResourceModelMap *resourceMap, QObject *parent);

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QModelIndex insert(const QModelIndex &parent, int row, buffers::TreeNode *node);
  QModelIndex addNode(buffers::TreeNode *child, const QModelIndex &parent);
  buffers::TreeNode *duplicateNode(const buffers::TreeNode &node);
  void removeNode(const QModelIndex &index);
  void sortByName(const QModelIndex &index);

 private:
  buffers::TreeNode *root;
  ResourceModelMap *resourceMap;

  inline QString treeNodeMime() const { return QStringLiteral("RadialGM/buffers.TreeNode"); }
};

#endif  // TREEMODEL_H
