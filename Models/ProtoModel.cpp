#include "ProtoModel.h"
#include "MessageModel.h"
#include "RepeatedImageModel.h"
#include "RepeatedMessageModel.h"
#include "RepeatedModel.h"

#include "Components/Logger.h"

ProtoModel::ProtoModel(QObject *parent, Message *protobuf) :
  ProtoModel(static_cast<ProtoModel *>(nullptr), protobuf) {
  QObject::setParent(parent);
}

ProtoModel::ProtoModel(ProtoModel *parent, Message *protobuf)
    : QAbstractItemModel(parent), _dirty(false), _protobuf(protobuf), _parentModel(parent),
      _desc(protobuf->GetDescriptor()), _refl(protobuf->GetReflection()) {
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
  if (!index.isValid()) return nullptr;
  auto flags = QAbstractItemModel::flags(index);
  flags |= Qt::ItemIsEditable;
  return flags;
}
