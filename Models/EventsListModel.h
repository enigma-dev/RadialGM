#ifndef EVENTSLISTSMODEL_H
#define EVENTSLISTSMODEL_H

#include "event_reader/event_parser.h"

#include "RepeatedMessageModel.h"

#include <QIdentityProxyModel>

class EventsListModel : public QIdentityProxyModel {
 private:
   EventData* eventData_;

 public:
  EventsListModel(EventData* eventData, QObject* parent = nullptr);
  Event GetEvent(const QModelIndex &index) const;

   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
   QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
   int columnCount(const QModelIndex &parent = QModelIndex()) const override;
};

#endif // EVENTSLISTS_H
