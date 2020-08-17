#ifndef EVENTSLISTSMODEL_H
#define EVENTSLISTSMODEL_H

#include "event_reader/event_parser.h"

#include <QAbstractProxyModel>
#include <QMap>

class EventsListModel : public QAbstractProxyModel {
 private:
    EventData* eventData_;
    QMap<QString, QPair<int, QSet<QString>>> eventGroups_;

 public:
  EventsListModel(EventData* eventData, QObject* parent = nullptr) :
        QAbstractProxyModel(parent), eventData_(eventData) {}

   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
   QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
   QModelIndex parent(const QModelIndex &index) const override;
   int rowCount(const QModelIndex &parent = QModelIndex()) const override;
   int columnCount(const QModelIndex &parent = QModelIndex()) const override;
   QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
   QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

   const Event GetEvent(const QModelIndex &index) const;

};

#endif // EVENTSLISTS_H
