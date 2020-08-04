#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "ResourceModelMap.h"
#include "ProtoModel.h"
#include "Components/ArtManager.h"
#include "treenode.pb.h"

#include <QHash>
#include <QSortFilterProxyModel>

#include <unordered_map>

using TypeCase = buffers::TreeNode::TypeCase;

class TreeModel : public QSortFilterProxyModel {
  Q_OBJECT

 public:
  explicit TreeModel(ProtoModel *source, ResourceModelMap *resourceMap, QObject *parent);

  virtual QModelIndex mapToSource(const QModelIndex &sourceIndex) const override;
  virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
  virtual QVariant data(const QModelIndex &index, int role) const override;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  QModelIndex insert(const QModelIndex &parent, int row, buffers::TreeNode *node);
  QModelIndex addNode(buffers::TreeNode *child, const QModelIndex &parent);
  buffers::TreeNode *duplicateNode(const buffers::TreeNode &node);
  void sort(const QModelIndex &index);

 private:
  ProtoModel *protoModel;
  ResourceModelMap *resourceMap;
};

#endif  // TREEMODEL_H
