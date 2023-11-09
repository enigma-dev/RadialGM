#include "SystemsModel.h"

SystemsModel::SystemsModel(buffers::SystemType system, QObject *parent) : QAbstractListModel(parent), system_(system)
{

}

QVariant SystemsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return QVariant();

  switch (role) {
    case Qt::DisplayRole: return QString::fromStdString(system_.subsystems(index.row()).name());
    //case Qt::ToolTipRole: return QString::fromStdString(system_.subsystems(index.row()).description());
    default: return QVariant();
  }
}

int SystemsModel::columnCount(const QModelIndex &/*parent*/) const {
  return 1;
}

int SystemsModel::rowCount(const QModelIndex& /*parent*/) const {
  return system_.subsystems_size();
}
