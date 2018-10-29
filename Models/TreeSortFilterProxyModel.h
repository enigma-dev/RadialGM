#ifndef TREESORTFILTERPROXYMODEL_H
#define TREESORTFILTERPROXYMODEL_H

#include "Models/ProtoModel.h"

#include <QSortFilterProxyModel>

class TreeSortFilterProxyModel : public QSortFilterProxyModel
{
public:
  TreeSortFilterProxyModel(QObject *parent = nullptr);
  void SetFilterType(TreeNode::TypeCase type);

protected:
  TreeNode::TypeCase filterType = TreeNode::TypeCase::TYPE_NOT_SET;
  bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // TREESORTFILTERPROXYMODEL_H
