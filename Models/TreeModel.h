#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "ProtoModel.h"
#include "Components/ArtManager.h"
#include "treenode.pb.h"

#include <QHash>
#include <QSortFilterProxyModel>

#include <unordered_map>

using TreeNode = buffers::TreeNode;
using TypeCase = TreeNode::TypeCase;

class TreeModel : public QSortFilterProxyModel {
  Q_OBJECT

 public:
  explicit TreeModel(ProtoModel *source, QObject *parent);
  void setSourceModel(QAbstractItemModel *sourceModel) override;

  // basic abstract item model stuff we override to organize the
  // super model to only show the tree nodes
  virtual QModelIndex index(int row, int column,
                            const QModelIndex &parent = QModelIndex()) const override;
  virtual QModelIndex parent(const QModelIndex &index) const override;
  virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
  virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
  virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  // these are all forwarded to the repeated child field
  //TODO: Does Josh know a way to template this shit?
  inline QModelIndex getRepeatedChildFieldIndex(const QModelIndex& treeNodeIndex) const {
    auto sourceIndex = mapToSource(treeNodeIndex);
    return protoModel->index(14, 0, sourceIndex);
  }
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override {
    return protoModel->rowCount(getRepeatedChildFieldIndex(parent));
  }
  virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const override {
    return protoModel->hasChildren(getRepeatedChildFieldIndex(parent));
  }
  virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override {
    return protoModel->insertRows(row, count, getRepeatedChildFieldIndex(parent));
  }
  virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                        const QModelIndex &destinationParent, int destinationChild) override {
    return protoModel->moveRows(getRepeatedChildFieldIndex(sourceParent), sourceRow, count,
                                getRepeatedChildFieldIndex(destinationParent), destinationChild);
  }
  virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override {
    return protoModel->removeRows(row, count, getRepeatedChildFieldIndex(parent));
  }
  virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                               const QModelIndex &parent) const override {
    return protoModel->canDropMimeData(data, action, row, column,
                                       getRepeatedChildFieldIndex(parent));
  }
  virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                            const QModelIndex &parent) override {
    return protoModel->dropMimeData(data, action, row, column,
                                    getRepeatedChildFieldIndex(parent));
  }

  // our public API for manipulating the tree or the super model
  // through the tree and for mapping resources
  QModelIndex addNode(const QModelIndex &parent);
  buffers::TreeNode *duplicateNode(const buffers::TreeNode &node);
  void sort(const QModelIndex &index);

 private:
  ProtoModel *protoModel;
};

#endif  // TREEMODEL_H
