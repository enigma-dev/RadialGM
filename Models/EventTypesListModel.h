#ifndef EVENTTYPESSLISTSMODEL_H
#define EVENTTYPESSLISTSMODEL_H

#include "RepeatedMessageModel.h"
#include "event_reader/event_parser.h"
#include "EventDescriptor.pb.h"

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QIcon>

class EventTypesListModel : public QAbstractListModel {
 private:
   EventData* eventData_;

 public:
   EventTypesListModel(EventData* eventData, QObject* parent = nullptr) :
     QAbstractListModel(parent), eventData_(eventData) {

   }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
    if (!index.isValid()) return QVariant();

    switch (role) {
      case Qt::DecorationRole: {
        QIcon icon(":/events/" + QString::fromStdString(eventData_->events()[index.row()].bare_id()).toLower() + ".png");
        if (!icon.availableSizes().empty()) return icon;
        return QIcon(":/events/other.png");
      }

      case Qt::DisplayRole: {
        QString str = QString::fromStdString(eventData_->events()[index.row()].HumanName());
        while (str.contains("%")) {
          str = str.arg("");
        }
        return str;
      }

      case Qt::ToolTipRole: return QString::fromStdString(eventData_->events()[index.row()].HumanDescription());
      case Qt::UserRole: return eventData_->events()[index.row()].event->type() != 5; //FIXME: mark hidden events in ey
      case Qt::UserRole+1: return QString::fromStdString(eventData_->events()[index.row()].GroupName());
      default: return QVariant();

    }
  }

  int rowCount(const QModelIndex &parent = QModelIndex()) const override { return eventData_->events().size(); }
  int columnCount(const QModelIndex &parent = QModelIndex()) const override { return 1; }

};

class EventTypesListSortFilterProxyModel : public QSortFilterProxyModel {
public:
  EventTypesListSortFilterProxyModel(QObject* parent = nullptr) : QSortFilterProxyModel(parent) {}
  bool filterAcceptsRow(int source_row, const QModelIndex& /*source_parent*/) const override {
    return sourceModel()->data(sourceModel()->index(source_row, 0), Qt::UserRole).toBool();
  }
  bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override {
    //FIXME: trying to sort alpha but keep grouped events together
    QString leftGroup = sourceModel()->data(source_left, Qt::UserRole+1).toString();
    QString rightGroup = sourceModel()->data(source_right, Qt::UserRole+1).toString();

    if (leftGroup == rightGroup) {
      return sourceModel()->data(source_left, Qt::DisplayRole).toString() <
          sourceModel()->data(source_right, Qt::DisplayRole).toString();
    }

    //if (leftGroup.isEmpty() && !rightGroup.isEmpty()) return false;
    return leftGroup < rightGroup;
  }
};

#endif // EVENTTYPESSLISTS_H
