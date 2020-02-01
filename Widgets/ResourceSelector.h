#ifndef RESOURCESELECTOR_H
#define RESOURCESELECTOR_H

#include "Components/QMenuView.h"
#include "MainWindow.h"
#include "Models/TreeSortFilterProxyModel.h"

#include <QToolButton>

class ResourceSelector : public QToolButton {
 public:
  explicit ResourceSelector(QWidget* parent, TreeNode::TypeCase type = TreeNode::TypeCase::kObject)
      : QToolButton(parent) {
    _menu = new QMenuView(this);
    _treeProxy = new TreeSortFilterProxyModel(this);
    _treeProxy->SetFilterType(type);
    _treeProxy->setSourceModel(MainWindow::treeModel.get());
    _menu->setModel(_treeProxy);
    setMenu(_menu);
  }

  void SetResourceType(TreeNode::TypeCase type) { _treeProxy->SetFilterType(type); }

  QMenuView* _menu;

 protected:
  TreeSortFilterProxyModel* _treeProxy;
};

#endif  // RESOURCESELECTOR_H
