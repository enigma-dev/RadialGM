#include "TreeModel.h"

#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "Models/ResourceModelMap.h"

#include "project.pb.h"

#include <QCoreApplication>
#include <QMimeData>

TreeModel::TreeModel(ProtoModel *source, ResourceModelMap *resourceMap, QObject *parent)
    : QSortFilterProxyModel(parent), protoModel(source), resourceMap(resourceMap) {
  this->setSourceModel(source);
  this->setFilterRole(Qt::DisplayRole);
  this->setFilterKeyColumn(0);

  this->setRecursiveFilteringEnabled(true);
  //this->setFilterFixedString("timeline");
}

QModelIndex TreeModel::mapToSource(const QModelIndex &proxyIndex) const {
  auto hrm = QSortFilterProxyModel::mapToSource(proxyIndex);

  if (!proxyIndex.isValid()) {
    auto *lol = (buffers::Project*)protoModel->GetMessage(QModelIndex());
    auto wtf = lol->mutable_game()->mutable_root();
    auto p = protoModel->indexOfFieldByNumber(wtf, buffers::TreeNode::kChildFieldNumber);
    auto omg = protoModel->index(0,0,protoModel->index(0,0,protoModel->index(0,0,hrm)));
    return protoModel->index(14,0,protoModel->index(0,0,omg));
    //return protoModel->index(0,0,hrm);
    //return QSortFilterProxyModel::mapToSource(proxyIndex.parent());
  } else if (protoModel->IsMessage(hrm)) {
    auto msg = protoModel->GetMessage(hrm);
    if (msg->GetTypeName() == "buffers.TreeNode") {
      hrm = protoModel->index(14,0,hrm);
    }
  }

  return hrm;
}


bool TreeModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
  return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
  QModelIndex sourceIndex = sourceModel()->index(source_row,0,source_parent);
  if (protoModel->IsMessage(sourceIndex) &&
      protoModel->GetMessage(sourceIndex)->GetTypeName() == "buffers.TreeNode") {
    //QModelIndex sourceFieldIndex = sourceModel()->index(14,0,sourceIndex);
    return QSortFilterProxyModel::filterAcceptsRow(14, sourceIndex);
  }
  return false;
  return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
  QModelIndex sourceIndex = QSortFilterProxyModel::mapToSource(index);
  if (protoModel->IsMessage(sourceIndex) &&
      protoModel->GetMessage(sourceIndex)->GetTypeName() == "buffers.TreeNode") {
    return protoModel->data(sourceIndex,role);
  }
  return QSortFilterProxyModel::data(index,role);
}

QVariant TreeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  return tr("Name");
}

QModelIndex TreeModel::insert(const QModelIndex &parent, int row, buffers::TreeNode *node) {

}

QModelIndex TreeModel::addNode(buffers::TreeNode *child, const QModelIndex &parent) {

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

void TreeModel::sortByName(const QModelIndex &index) {
  if (!index.isValid()) return;
  auto *node = static_cast<buffers::TreeNode *>(index.internalPointer());
  if (!node) return;
  auto *child_field = node->mutable_child();
  std::sort(child_field->begin(), child_field->end(),
            [](const buffers::TreeNode &a, const buffers::TreeNode &b) { return a.name() < b.name(); });
  emit dataChanged(this->index(0, 0, index), this->index(node->child_size(), 0, index));
}
