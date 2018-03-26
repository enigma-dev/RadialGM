#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "treenode.pb.h"

#include <QAbstractItemModel>

class TreeModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  explicit TreeModel(buffers::TreeNode *root, QObject *parent);

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

 private:
  //void setupModelData(const QStringList &lines, TreeItem *parent);

  buffers::TreeNode *root;
};

#endif  // TREEMODEL_H
