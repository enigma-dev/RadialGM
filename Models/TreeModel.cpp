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

  //this->setRecursiveFilteringEnabled(true);
  //this->setFilterFixedString("timeline");
}

void TreeModel::setSourceModel(QAbstractItemModel *sourceModel) {
  //TODO: Sanity check that sourceModel is a ProtoModel
  this->protoModel = static_cast<ProtoModel*>(sourceModel);
  QSortFilterProxyModel::setSourceModel(sourceModel);
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
  auto index = QSortFilterProxyModel::index(row, column, parent);
  auto sourceIndex = mapToSource(index);
  auto sourceParent = mapToSource(parent); // << parent was already mapped

  if (protoModel->IsMessage(sourceParent) &&
      protoModel->GetMessage(sourceParent)->GetTypeName() == "buffers.TreeNode") {
    auto childIndex = getRepeatedChildFieldIndex(parent);
    return mapFromSource(protoModel->index(row, column, childIndex));
  }

  return QModelIndex(); // reject everything else
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
  auto sourceIndex = mapToSource(index);

  if (protoModel->IsMessage(sourceIndex)) {
    auto msg = protoModel->GetMessage(sourceIndex);
    if (msg->GetTypeName() == "buffers.TreeNode") {
      auto backAgain = mapFromSource(sourceIndex.parent().parent());
      return backAgain;
    }
  }

  return QModelIndex(); // << reject everything else
}

QModelIndex TreeModel::mapFromSource(const QModelIndex &sourceIndex) const {
  //return QSortFilterProxyModel::mapFromSource(sourceIndex);
  //qDebug() << "mapFrom" << sourceIndex;
  auto omg = protoModel->index(0,0,protoModel->index(0,0,protoModel->index(0,0,QModelIndex())));
  omg = protoModel->index(0,0,omg);
  if (!sourceIndex.isValid()) {
      //return QModelIndex();
  }
  if (sourceIndex == omg) {
    return QModelIndex(); // this is our tree root
  }

  return QSortFilterProxyModel::mapFromSource(sourceIndex);
}

QModelIndex TreeModel::mapToSource(const QModelIndex &proxyIndex) const {
  //return QSortFilterProxyModel::mapToSource(proxyIndex);
  if (!proxyIndex.isValid()) {
    //auto *lol = (buffers::Project*)protoModel->GetMessage(QModelIndex());
    //auto wtf = lol->mutable_game()->mutable_root();
    //auto p = protoModel->indexOfFieldByNumber(wtf, buffers::TreeNode::kChildFieldNumber);
    auto omg = protoModel->index(0,0,protoModel->index(0,0,protoModel->index(0,0,QModelIndex())));
    omg = protoModel->index(0,0,omg);
    return omg;
  }

  return QSortFilterProxyModel::mapToSource(proxyIndex);
}

bool TreeModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
  return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
  QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
  if (protoModel->IsMessage(sourceIndex) &&
      protoModel->GetMessage(sourceIndex)->GetTypeName() == "buffers.TreeNode") {
    //mapToSource(mapFromSource(source_parent));
    qDebug() << "filterAccepts" << source_row << source_parent;
    return QSortFilterProxyModel::filterAcceptsRow(source_row, sourceIndex);
  }
  qDebug() << "filterRejects" << source_row << source_parent;
  return false;
  return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

QVariant TreeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  return tr("Name");
}

QModelIndex TreeModel::addNode(const QModelIndex &parent) {
  auto insertParent = parent;
  int pos = rowCount(parent);

  if (parent.isValid()) { // << not the root
    //TreeNode *parentNode = static_cast<TreeNode *>(parent.internalPointer());
    //if (!parentNode->has_folder()) { // << not a folder
      //TODO: FIXME
      //insertParent = insertParent.parent();
      //pos = parent.row();
    //}
  }

  insertRow(pos,insertParent);
  return index(pos, 0, insertParent);
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

void TreeModel::sort(const QModelIndex &index) {
  auto *node = static_cast<buffers::TreeNode *>(index.internalPointer());
  if (!node) return;
  auto *child_field = node->mutable_child();
  std::sort(child_field->begin(), child_field->end(),
            [](const buffers::TreeNode &a, const buffers::TreeNode &b) { return a.name() < b.name(); });
  emit dataChanged(this->index(0, 0, index), this->index(node->child_size(), 0, index));
}
