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
    menu = new QMenuView(this);
    treeProxy = new TreeSortFilterProxyModel(this);
    treeProxy->SetFilterType(type);
    treeProxy->setSourceModel(MainWindow::treeModel.get());
    menu->setModel(treeProxy);
    setMenu(menu);
  }

  void SetResourceType(TreeNode::TypeCase type) { treeProxy->SetFilterType(type); }

  QMenuView* menu;

 protected:
  TreeSortFilterProxyModel* treeProxy;
};

#endif  // RESOURCESELECTOR_H
