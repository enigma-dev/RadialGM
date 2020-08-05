#include "EditorModel.h"

EditorModel::EditorModel(const QPersistentModelIndex &protoRoot, QObject *parent) :
  QIdentityProxyModel(parent), protoRoot(protoRoot) {}

QModelIndex EditorModel::mapToSource(const QModelIndex &sourceIndex) const {
  if (!sourceIndex.isValid())
    return protoRoot;

  return QIdentityProxyModel::mapToSource(sourceIndex);
}

QModelIndex EditorModel::mapFromSource(const QModelIndex &sourceIndex) const {
  if (sourceIndex == protoRoot)
    return QModelIndex();

  return QIdentityProxyModel::mapFromSource(sourceIndex);
}
