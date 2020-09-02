#ifndef EVENTTYPESLISTSORTFILTERPROXYMODEL_H
#define EVENTTYPESLISTSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class EventTypesListSortFilterProxyModel : public QSortFilterProxyModel {
 public:
  EventTypesListSortFilterProxyModel(QObject* parent = nullptr);
  bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
  bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;
};

#endif  // EVENTTYPESLISTSORTFILTERPROXYMODEL_H
