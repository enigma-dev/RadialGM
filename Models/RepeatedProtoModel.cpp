#include "RepeatedProtoModel.h"
#include "ProtoModel.h"

RepeatedProtoModel::RepeatedProtoModel(Message *protobuf, const FieldDescriptor *field, ProtoModel *parent) :
   QAbstractItemModel(parent), protobuf(protobuf), field(field) {}

int RepeatedProtoModel::rowCount(const QModelIndex& /*parent*/) const {
  const Reflection *refl = protobuf->GetReflection();
  return refl->FieldSize(*protobuf, field);
}

int RepeatedProtoModel::columnCount(const QModelIndex& /*parent*/) const {
  const Descriptor *desc = protobuf->GetDescriptor();
  return desc->field_count();
}

//bool RepeatedProtoModel::setData(const QModelIndex &index, const QVariant &value, int role) {}

QVariant RepeatedProtoModel::data(int row, int column) const {
  return data(this->index(row, column, QModelIndex()), Qt::DisplayRole);
}

QVariant RepeatedProtoModel::data(const QModelIndex &index, int role) const {
  if (role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();
  ProtoModel* m = static_cast<ProtoModel*>(QObject::parent())->GetSubModel(field->number(), index.row());
  return m->data(index.column());
}

QModelIndex RepeatedProtoModel::parent(const QModelIndex& index) const {
  return QModelIndex();
}

QVariant RepeatedProtoModel::headerData(int section, Qt::Orientation orientation, int role) const {
  ProtoModel* m = static_cast<ProtoModel*>(QObject::parent())->GetSubModel(field->number(), 0);
  return m->headerData(section, orientation, role);
}

QModelIndex RepeatedProtoModel::index(int row, int column, const QModelIndex &parent) const {
  return createIndex(row, column);
}

Qt::ItemFlags RepeatedProtoModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return nullptr;
  return QAbstractItemModel::flags(index);
}
