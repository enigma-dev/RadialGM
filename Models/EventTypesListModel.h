#ifndef EVENTTYPESSLISTSMODEL_H
#define EVENTTYPESSLISTSMODEL_H

#include "RepeatedMessageModel.h"
#include "event_reader/event_parser.h"

#include <QSortFilterProxyModel>

class EventTypesListModel : public QSortFilterProxyModel {
 private:
   EventData* eventData_;

 public:
   EventTypesListModel(EventData* eventData, QObject* parent = nullptr) :
     QSortFilterProxyModel(parent), eventData_(eventData) {

   }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
    if (!index.isValid()) return QVariant();
    if (role == Qt::DisplayRole) {
      return QString::fromStdString(eventData_->events()[index.row()].bare_id());
    }
    return QVariant();
  };
  int rowCount(const QModelIndex &parent = QModelIndex()) const override { return eventData_->events().size(); }
  int columnCount(const QModelIndex &parent = QModelIndex()) const override { return 1; }

};

#endif // EVENTTYPESSLISTS_H
