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

  Event eventDesc = eventData_->get_event(name, arguments_vec);

  QString group = QString::fromStdString(eventDesc.bare_id());
  QString fullname = QString::fromStdString(eventDesc.HumanName());
  //if (eventGroups_.contains(group)) {
    //Qt's const fuxing me
    //eventGroups_[group] = QPair<int, QSet<QString>>(eventGroups_.size(), QSet<QString>());
  //}
  //eventGroups_[group].second.insert(fullname);

  return eventDesc;
}

EventsListModel::EventsListModel(EventData* eventData, QObject* parent) :
  QAbstractProxyModel(parent), eventData_(eventData) {
}

void EventsListModel::setSourceModel(QAbstractItemModel* model) {
  QAbstractProxyModel::setSourceModel(model);
  int group = 0;
  auto lastEvent = GetEvent(createIndex(0,0));
  groupRowStart[group] = 0;
  groupRowCount[group]++;
  for (int i = 1; i < sourceModel()->rowCount(QModelIndex()); ++i) {
    auto event = GetEvent(createIndex(i,0));
    if (event.bare_id() != lastEvent.bare_id()) {
      lastEvent = event;
      groupRowStart[++group] = i;
    }
    ++groupRowCount[group];
  }
  qDebug() << groupRowStart << groupRowCount;
}

QVariant EventsListModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole || section > 0) return QVariant();
  return tr("Events");
}

QVariant EventsListModel::data(const QModelIndex &index, int role) const {
  if (role != Qt::DisplayRole && role != Qt::DecorationRole) return QVariant();
  if (!index.isValid()) return QVariant(); // << invisible root

  if (IsEventGroup(index)) { // << group node
    auto sourceStart = groupRowStart[index.row()];
    Event event = GetEvent(createIndex(sourceStart,0));
    auto rowCount = groupRowCount[index.row()];
    if (rowCount > 1) {
      if (role == Qt::DecorationRole)
        return QIcon(":/events/" + QString::fromStdString(event.bare_id()).toLower() + "-folder.png");
      return QString::fromStdString(event.bare_id());
    }
    if (role == Qt::DecorationRole)
      return QIcon(":/events/" + QString::fromStdString(event.bare_id()).toLower() + ".png");
    return QString::fromStdString(event.HumanName());
  } else { // << regular event node
    auto parentRow = index.internalId()-1;
    auto sourceStart = groupRowStart[parentRow]+index.row();
    Event event = GetEvent(createIndex(sourceStart,0));
    if (role == Qt::DecorationRole)
      return QIcon(":/events/" + QString::fromStdString(event.bare_id()).toLower() + ".png");
    return QString::fromStdString(event.HumanName());
  }

  return QVariant(); // << unknown
}

QModelIndex EventsListModel::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  if (!parent.isValid()) // << group
    createIndex(row, column);

  return createIndex(row, column, parent.row()+1);
}

QModelIndex EventsListModel::parent(const QModelIndex& index) const {
  //QString group = data(index, Qt::UserRole).toString();

  if (index.internalId()) { // << regular event
    auto parentRow = index.internalId()-1;
    return createIndex(parentRow, 0);
  }

  return QModelIndex();
}

int EventsListModel::rowCount(const QModelIndex &parent) const {
  if (parent.column() > 0) return 0;

  if (!parent.isValid()) return groupRowStart.size();
  if (parent.parent().isValid()) return 0;

  auto rows = groupRowCount[parent.row()];
  if (rows > 1) return rows;
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
