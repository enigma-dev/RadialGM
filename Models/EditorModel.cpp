#include "EditorModel.h"

EditorModel::EditorModel(const QModelIndex &protoRoot, QObject *parent) :
  QSortFilterProxyModel(parent), _backup(nullptr), _protoRoot(protoRoot) {}

QModelIndex EditorModel::mapToSource(const QModelIndex &proxyIndex) const {
  // our root is a tree node
  if (!proxyIndex.isValid())
    return _protoRoot;

  return QSortFilterProxyModel::mapToSource(proxyIndex);
}

QModelIndex EditorModel::mapFromSource(const QModelIndex &sourceIndex) const {
  // tree node is our root
  if (sourceIndex == _protoRoot)
    return QModelIndex();

  return QSortFilterProxyModel::mapFromSource(sourceIndex);
}

// revert our backup to the super model
void EditorModel::revert() {
  // ask the source for a message pointer
  auto ptr = data(QModelIndex(),Qt::UserRole+1).value<void*>();
  auto msg = static_cast<Message*>(ptr);
  // we edit the super source model directly so
  // we only need to make a fresh backup
  if (!_backup) _backup.reset(msg->New());
  _backup->CopyFrom(*msg);
}

// push our backup to the super model
bool EditorModel::submit() {
  if (!_backup) return false;

  // ask the source for a message pointer
  auto ptr = data(QModelIndex(),Qt::UserRole+1).value<void*>();
  auto msg = static_cast<Message*>(ptr);
  // restore the super model to what the user had before editing
  beginResetModel();
  msg->CopyFrom(*_backup);
  endResetModel();
  return true;
}
