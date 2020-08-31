#include "ProtoModel.h"
#include "MessageModel.h"
#include "RepeatedImageModel.h"
#include "RepeatedMessageModel.h"
#include "RepeatedModel.h"

#include "Components/Logger.h"

ProtoModel::ProtoModel(QObject *parent, Message *protobuf) : ProtoModel(static_cast<ProtoModel *>(nullptr), protobuf) {
  QObject::setParent(parent);
}

ProtoModel::ProtoModel(ProtoModel *parent, Message *protobuf)
    : QAbstractItemModel(parent), _dirty(false), _protobuf(protobuf), _parentModel(parent) {
  connect(this, &ProtoModel::DataChanged, this,
          [this](const QModelIndex &topLeft, const QModelIndex &bottomRight,
                 const QVariant & /*oldValue*/ = QVariant(0), const QVector<int> &roles = QVector<int>()) {
            emit QAbstractItemModel::dataChanged(topLeft, bottomRight, roles);
          });
}

void ProtoModel::ParentDataChanged() {
  ProtoModel *m = GetParentModel<ProtoModel *>();
  while (m != nullptr) {
    emit m->DataChanged(m->index(0, 0), m->index(rowCount() - 1, columnCount() - 1));
    m = m->GetParentModel<ProtoModel *>();
  }
}

void ProtoModel::SetDirty(bool dirty) { _dirty = dirty; }

bool ProtoModel::IsDirty() { return _dirty; }

QModelIndex ProtoModel::parent(const QModelIndex & /*index*/) const { return QModelIndex(); }

Qt::ItemFlags ProtoModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return Qt::NoItemFlags;
  auto flags = QAbstractItemModel::flags(index);
  flags |= Qt::ItemIsEditable;
  return flags;
}

bool ProtoModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) {
  if (orientation == Qt::Horizontal) {
    _horizontalHeaderData[section][static_cast<Qt::ItemDataRole>(role)] = value;
    emit headerDataChanged(Qt::Horizontal, section, section);
    return true;
  } else if (orientation == Qt::Vertical) {
    _verticalHeaderData[section][static_cast<Qt::ItemDataRole>(role)] = value;
    emit headerDataChanged(Qt::Vertical, section, section);
    return true;
  }
  return QAbstractItemModel::setHeaderData(section, orientation, value, role);
}

static QVariant getHeaderData(const QHash<int,QHash<Qt::ItemDataRole,QVariant>>& map, int section, int role) {
  auto sit = map.find(section);
  if (sit == map.end()) return QVariant();
  auto sectionMap = *sit;
  auto it = sectionMap.find(static_cast<Qt::ItemDataRole>(role));
  if (it == sectionMap.end()) return QVariant();
  return *it;
}

QVariant ProtoModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal) {
    return getHeaderData(_horizontalHeaderData, section, role);
  } else if (orientation == Qt::Vertical) {
    return getHeaderData(_verticalHeaderData, section, role);
  }
  return QVariant();
}
