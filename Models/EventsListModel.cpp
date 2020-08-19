#include "EventsListModel.h"
#include "MessageModel.h"
#include "RepeatedStringModel.h"
#include "RepeatedMessageModel.h"

#include <QIcon>

static inline bool IsEventGroup(const QModelIndex &index) {
  if (!index.isValid()) return true; // << invisible root is a group
  if (!index.parent().isValid()) return true; // << root immediate children are groups
  return false; // << regular events have valid parents & grandparents
}

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
  QAbstractProxyModel(parent), eventData_(eventData) {
}

void EventsListModel::setSourceModel(QAbstractItemModel* model) {
  QAbstractProxyModel::setSourceModel(model);

  // For each event in source model
  for (int i = 0; i < sourceModel()->rowCount(QModelIndex()); ++i) {
    // Get the event info from the source model
    const Event& event = GetEvent(createIndex(i, 0));
    QString group = QString::fromStdString(event.bare_id());

    // Look for a group matching the current event
    int groupIndex = 0;
    for (auto& idx : modelEvents_) {
      if (idx.first == group) {
        break;
      }
      groupIndex++;
    }

    // If no matching group found add one
    if (groupIndex >= modelEvents_.size()) {
      groupIndex = modelEvents_.size();
      modelEvents_.push_back({ group, {} });
      // Store the position of the group in the vector in a map for quick lookups
      groupIDs_[group] = groupIndex;
    }

    // Add the event to the group we found or added above
    modelEvents_[groupIndex].second.push_back(QString::fromStdString(event.HumanName()));
  }
}

QVariant EventsListModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole || section > 0) return QVariant();
  return tr("Events");
}

QVariant EventsListModel::data(const QModelIndex &index, int role) const {
  if (role != Qt::DisplayRole /*&& role != Qt::DecorationRole*/) return QVariant();
  if (!index.isValid()) return QVariant(); // << invisible root

    qDebug() << index.row() << "," << index.column() << Qt::endl;
  //return modelEvents_[index.row()].second[index.column()];
}

QModelIndex EventsListModel::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  //if (!parent.isValid()) // << group
    //createIndex(row, column);

  return createIndex(row, column);
}

QModelIndex EventsListModel::parent(const QModelIndex& index) const {
  return QModelIndex();
}

int EventsListModel::rowCount(const QModelIndex &parent) const {
  return 0;
}

int EventsListModel::columnCount(const QModelIndex &parent) const {
  return 1;
}

Qt::ItemFlags EventsListModel::flags(const QModelIndex &index) const {
  Qt::ItemFlags itemFlags = QAbstractItemModel::flags(index);
  auto rows = rowCount(index);
  if (rows > 0)
    itemFlags.setFlag(Qt::ItemIsSelectable, false);
  return itemFlags;
}

QModelIndex EventsListModel::mapFromSource(const QModelIndex &sourceIndex) const {
  QString group = data(sourceIndex, Qt::UserRole).toString();
  return QModelIndex(); // ??? index(eventGroups_[group].first,
}

QModelIndex EventsListModel::mapToSource(const QModelIndex &proxyIndex) const {
  return QModelIndex(); // ???
}
