#ifndef EVENTSLISTSMODEL_H
#define EVENTSLISTSMODEL_H

#include "event_reader/event_parser.h"

#include "RepeatedMessageModel.h"

#include <QAbstractProxyModel>
#include <QMap>

class EventsListModel : public QAbstractProxyModel {
 private:
    EventData* eventData_;

    QMap<size_t, size_t> groupRowStart;
    QMap<size_t, size_t> groupRowCount;

 public:
  EventsListModel(EventData* eventData, QObject* parent = nullptr);

   Qt::ItemFlags flags(const QModelIndex &index) const override;
   void setSourceModel(QAbstractItemModel* model) override;
   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
   QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
   QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
   QModelIndex parent(const QModelIndex &index) const override;
   int rowCount(const QModelIndex &parent = QModelIndex()) const override;
   bool hasChildren(const QModelIndex &parent = QModelIndex()) const override {
     return rowCount(parent) > 0;
   }
   int columnCount(const QModelIndex &parent = QModelIndex()) const override;
   QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
   QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

   Event GetEvent(const QModelIndex &index) const;

};

#endif // EVENTSLISTS_H
