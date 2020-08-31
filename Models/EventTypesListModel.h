#ifndef EVENTTYPESSLISTSMODEL_H
#define EVENTTYPESSLISTSMODEL_H

#include "EventDescriptor.pb.h"
#include "RepeatedMessageModel.h"
#include "event_reader/event_parser.h"

#include <QAbstractListModel>
#include <QIcon>
#include <QSortFilterProxyModel>

class EventTypesListModel : public QAbstractListModel {
 private:
  EventData* eventData_;

 public:
  static enum UserRoles {
    EventTypeRole = Qt::UserRole,
    EventGroupRole,
    EventArgumentsRole,
    EventBareIDRole
  } useroles;

  EventTypesListModel(EventData* eventData, QObject* parent = nullptr);
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
};

#endif  // EVENTTYPESSLISTS_H
