#include "EventTypesListSortFilterProxyModel.h"
#include "EventTypesListModel.h"


EventTypesListSortFilterProxyModel::EventTypesListSortFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent) {}

bool EventTypesListSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& /*source_parent*/) const {
  return sourceModel()->data(sourceModel()->index(source_row, 0), Qt::UserRole).toBool();
}

bool EventTypesListSortFilterProxyModel::lessThan(const QModelIndex& source_left,
                                                  const QModelIndex& source_right) const {
  //Try to sort alpha but keep grouped events together
  QString leftGroup = sourceModel()->data(source_left, EventTypesListModel::UserRoles::EventGroupRole).toString();
  QString rightGroup = sourceModel()->data(source_right, EventTypesListModel::UserRoles::EventGroupRole).toString();

  if (leftGroup == rightGroup) {
    return sourceModel()->data(source_left, Qt::DisplayRole).toString() <
           sourceModel()->data(source_right, Qt::DisplayRole).toString();
  }

  return leftGroup < rightGroup;
}
