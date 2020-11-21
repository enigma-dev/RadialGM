#include "ResourceChangesModel.h"

#include <QIcon>
#include <QDebug>

ResourceChangesModel::ResourceChangesModel(QObject *parent) : QAbstractListModel(parent) {}

void ResourceChangesModel::ResourceChanged(Resource& res, ResChange change, const QString& oldName) {
  if (res.name.isEmpty()) return;

  beginResetModel();

  if (change == ResChange::Reverted) _changes.remove(res.name);

  if (_changes.contains(res.name)) {
    // Add then remove shouldn't be on list
    if (_changes[res.name] == ResChange::Added && change == ResChange::Removed) {
      _changes.remove(res.name);
      endResetModel();
      return;
    }
    // modified shouldnt overwrite added
    if (_changes[res.name] == ResChange::Added && change == ResChange::Modified) return;
    // modified shouldnt overwrite renamed
    if (_changes[res.name] == ResChange::Renamed && change == ResChange::Modified) return;
  }

  // if renamed before commited change the added name
  if (_changes[oldName] == ResChange::Added && change == ResChange::Renamed) {
    _changes.remove(oldName);
    _changes[res.name] = ResChange::Added;
    endResetModel();
    return;
  }

  _changes[res.name] = change;

  endResetModel();
}

void ResourceChangesModel::ClearChanges() {
  beginResetModel();
  _changes.clear();
  endResetModel();
}

int ResourceChangesModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  else
    return _changes.size();
}

QVariant ResourceChangesModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return QVariant();

  QList<QString> keys = _changes.keys();

  if (role == Qt::DecorationRole) {
    switch (_changes[keys[index.row()]]) {
      case ResChange::Added: return QIcon(":/actions/add.png");
      case ResChange::Modified: return QIcon(":/actions/edit.png");
      case ResChange::Renamed: return QIcon(":/actions/edit.png");
      case ResChange::Removed: return QIcon(":/actions/delete.png");
      case ResChange::Reverted: return QVariant();
    }
  } else if (role == Qt::DisplayRole) {
    switch (_changes[keys[index.row()]]) {
      case ResChange::Added: return keys[index.row()] + " " + tr("Added");
      case ResChange::Modified: return keys[index.row()] + " " + tr("Modified");
      case ResChange::Renamed: return keys[index.row()] + " " + tr("Renamed");
      case ResChange::Removed: return keys[index.row()] + " " + tr("Removed");
      case ResChange::Reverted: return QVariant();
    }
  }

  return QVariant();
}
