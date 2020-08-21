#ifndef EVENTSLISTSMODEL_H
#define EVENTSLISTSMODEL_H

#include "event_reader/event_parser.h"

#include "RepeatedMessageModel.h"

#include <QAbstractProxyModel>
#include <QMap>

class EventsListModel : public QAbstractProxyModel {
 private:
   EventData* eventData_;

 public:
  EventsListModel(EventData* eventData, QObject* parent = nullptr);
  Event GetEvent(const QModelIndex &index) const;

   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
   QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
   QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
   QModelIndex parent(const QModelIndex &index) const override;
   int rowCount(const QModelIndex &parent = QModelIndex()) const override;
   int columnCount(const QModelIndex &parent = QModelIndex()) const override;
   QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
   QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
};

#endif // EVENTSLISTS_H
