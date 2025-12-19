#include "EventTypesListModel.h"
#include <string_view>

EventTypesListModel::EventTypesListModel(EventData* eventData, QObject* parent)
    : QAbstractListModel(parent), eventData_(eventData) {}

QVariant EventTypesListModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) return QVariant();

  switch (role) {
    case Qt::DecorationRole: {
      std::string_view bareId = eventData_->events()[index.row()].bare_id();
      QIcon icon(":/events/" + QString::fromUtf8(bareId.data(), bareId.size()).toLower() + ".png");
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

    case Qt::ToolTipRole: {
      std::string desc = eventData_->events()[index.row()].HumanDescription();
      return QString::fromStdString(desc);
    }
    case EventTypeRole: return eventData_->events()[index.row()].event->type() != 5;  //FIXME: mark hidden events in ey
    case EventGroupRole: {
      std::string_view groupName = eventData_->events()[index.row()].GroupName();
      return QString::fromUtf8(groupName.data(), groupName.size());
    }
    case EventArgumentsRole: {
      QStringList args;
      for (std::string_view str : eventData_->events()[index.row()].event->parameters()) {
        args.append(QString::fromUtf8(str.data(), str.size()));
      }
      return args;
    }
    case EventBareIDRole: {
      std::string_view bareId = eventData_->events()[index.row()].bare_id();
      return QString::fromUtf8(bareId.data(), bareId.size());
    }

    default: return QVariant();
  }
}

int EventTypesListModel::rowCount(const QModelIndex& /*parent*/) const { return eventData_->events().size(); }
int EventTypesListModel::columnCount(const QModelIndex& /*parent*/) const { return 1; }
