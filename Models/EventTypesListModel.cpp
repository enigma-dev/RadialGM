#include "EventTypesListModel.h"

EventTypesListModel::EventTypesListModel(EventData* eventData, QObject* parent)
    : QAbstractListModel(parent), eventData_(eventData) {}

QVariant EventTypesListModel::data(const QModelIndex& index, int role) const {
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
    case EventTypeRole: return eventData_->events()[index.row()].event->type() != 5;  //FIXME: mark hidden events in ey
    case EventGroupRole: return QString::fromStdString(eventData_->events()[index.row()].GroupName());
    case EventArgumentsRole: {
      QStringList args;
      for (const auto& str : eventData_->events()[index.row()].event->parameters()) {
        args.append(QString::fromStdString(str));
      }
      return args;
    }
    case EventBareIDRole: {
      return QString::fromStdString(eventData_->events()[index.row()].bare_id());
    }

    default: return QVariant();
  }
}

int EventTypesListModel::rowCount(const QModelIndex& parent) const { return eventData_->events().size(); }
int EventTypesListModel::columnCount(const QModelIndex& parent) const { return 1; }
