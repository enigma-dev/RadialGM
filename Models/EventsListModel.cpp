#include "EventsListModel.h"
#include "MessageModel.h"
#include "RepeatedStringModel.h"
#include "RepeatedMessageModel.h"

const Event EventsListModel::GetEvent(const QModelIndex &index) const {
  std::string name = sourceModel()->data(index, Qt::DisplayRole).toString().toStdString();
  std::vector<std::string> arguments_vec;

  MessageModel* event =
  static_cast<RepeatedMessageModel*>(
              sourceModel())->GetSubModel<MessageModel*>(index.row());

  RepeatedStringModel* arguments = event->GetSubModel<RepeatedStringModel*>(Object::EgmEvent::kArgumentsFieldNumber);

  for (int i = 0; i < arguments->rowCount(); ++i) {
    arguments_vec.emplace_back(arguments->Data(i).toString().toStdString());
  }

  const Event eventDesc = eventData_->get_event(name, arguments_vec);

  QString group = QString::fromStdString(eventDesc.GroupName());
  QString fullname = QString::fromStdString(eventDesc.HumanName());
  if (eventGroups_.contains(group)) {
    //Qt's const fuxing me
    //eventGroups_[group] = QPair<int, QSet<QString>>(eventGroups_.size(), QSet<QString>());
  }
  //eventGroups_[group].second.insert(fullname);

  return eventDesc;
}

QVariant EventsListModel::data(const QModelIndex &index, int role) const {
  switch(role) {
    case Qt::DisplayRole: {
      const Event& event = GetEvent(index);
      return QString::fromStdString(event.HumanName());
    }
    case Qt::UserRole: {
      const Event& event = GetEvent(index);
      return QString::fromStdString(event.GroupName());
    }
    //case Qt::DecorationRole return QIcon();
    default: return sourceModel()->data(index, role);
  }
}

QModelIndex EventsListModel::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  //const Event& event = GetEvent(sourceModel()->index(row, column));
  //QString group = QString::fromStdString(event.GroupName());

  return createIndex(parent.row(), column);
}

QModelIndex EventsListModel::parent(const QModelIndex& index) const {
  QString group = data(index, Qt::UserRole).toString();
  if (!group.isEmpty())
    return createIndex(eventGroups_[group].first, 0);

  return QModelIndex();
}

int EventsListModel::rowCount(const QModelIndex &parent) const {
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid())
    return eventGroups_[data(parent, Qt::UserRole).toString()].second.size();

  return 0;
}

int EventsListModel::columnCount(const QModelIndex &parent) const {
  return parent.isValid();
}

QModelIndex EventsListModel::mapFromSource(const QModelIndex &sourceIndex) const {
  QString group = data(sourceIndex, Qt::UserRole).toString();
  return QModelIndex(); // ??? index(eventGroups_[group].first,
}

QModelIndex EventsListModel::mapToSource(const QModelIndex &proxyIndex) const {
  return QModelIndex(); // ???
}
