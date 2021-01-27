#include "EventsListModel.h"
#include "MessageModel.h"
#include "RepeatedModel.h"
#include "RepeatedMessageModel.h"

#include <QIcon>

Event EventsListModel::GetEvent(const QModelIndex &index) const {
  auto sourceIndex = sourceModel()->index(index.row(), Object::EgmEvent::kIdFieldNumber);
  std::string name = sourceModel()->data(sourceIndex,
                                         Qt::DisplayRole).toString().toStdString();
  std::vector<std::string> arguments_vec;

  MessageModel* event =
  static_cast<RepeatedMessageModel*>(
              sourceModel())->GetSubModel<MessageModel*>(index.row());

  RepeatedStringModel* arguments = event->GetSubModel<RepeatedStringModel*>(
        Object::EgmEvent::kArgumentsFieldNumber);

  for (int i = 0; i < arguments->rowCount(); ++i) {
    arguments_vec.emplace_back(arguments->Data(i).toString().toStdString());
  }

  return eventData_->get_event(name, arguments_vec);
}

EventsListModel::EventsListModel(EventData* eventData, QObject* parent) :
  QIdentityProxyModel(parent), eventData_(eventData) {
}

QVariant EventsListModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole || section > 0) return QVariant();
  return tr("Events");
}

QVariant EventsListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return QVariant(); // << invisible root

  Event event = GetEvent(index);

  switch (role) {
    case Qt::DisplayRole: return QString::fromStdString(event.HumanName());
    case Qt::DecorationRole: {
      QIcon icon(":/events/" + QString::fromStdString(event.bare_id()).toLower() + ".png");
      if (!icon.availableSizes().empty()) return icon;
      return QIcon(":/events/other.png");
    }
    case Qt::ToolTipRole: {
      MessageModel* event =
      static_cast<RepeatedMessageModel*>(
                  sourceModel())->GetSubModel<MessageModel*>(index.row());
      return event->Data(Object::EgmEvent::kCodeFieldNumber);
    }
    default: return QVariant();
  }
}

int EventsListModel::columnCount(const QModelIndex &/*parent*/) const {
  return 1;
}
