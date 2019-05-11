#include "RepeatedProtoModel.h"
#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "MainWindow.h"
#include "ProtoModel.h"
#include "ResourceModelMap.h"

RepeatedProtoModel::RepeatedProtoModel(Message *protobuf, const FieldDescriptor *field, ProtoModel *parent)
    : QAbstractItemModel(parent), protobuf(protobuf), field(field) {}

int RepeatedProtoModel::rowCount(const QModelIndex & /*parent*/) const {
  const Reflection *refl = protobuf->GetReflection();
  return refl->FieldSize(*protobuf, field);
}

bool RepeatedProtoModel::empty() const { return this->rowCount() <= 0; }

int RepeatedProtoModel::columnCount(const QModelIndex & /*parent*/) const {
  const Descriptor *desc = protobuf->GetDescriptor();
  return desc->field_count();
}

//bool RepeatedProtoModel::setData(const QModelIndex &index, const QVariant &value, int role) {}

QVariant RepeatedProtoModel::data(int row, int column) const {
  return data(this->index(row, column, QModelIndex()), Qt::DisplayRole);
}

QVariant RepeatedProtoModel::data(const QModelIndex &index, int role) const {
  R_EXPECT(index.isValid(), QVariant()) << "Supplied index was invalid:" << index;

  ProtoModel *m = static_cast<ProtoModel *>(QObject::parent())->GetSubModel(field->number(), index.row());
  QVariant data = m->data(index.column());
  if (role == Qt::DecorationRole && field->name() == "instances" &&
      index.column() == Room::Instance::kObjectTypeFieldNumber) {
    auto obj = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, data.toString());
    if (obj != nullptr) {
      obj = obj->GetSubModel(TreeNode::kObjectFieldNumber);
      ProtoModel *spr = MainWindow::resourceMap->GetResourceByName(
          TreeNode::kSprite, obj->data(Object::kSpriteNameFieldNumber).toString());
      static const QSize pixmapSize(18, 18);
      if (spr != nullptr) {
        spr = spr->GetSubModel(TreeNode::kSpriteFieldNumber);
        return ArtManager::GetIcon(spr->GetString(Sprite::kSubimagesFieldNumber, 0)).pixmap(pixmapSize);
      } else {
        return QIcon(":/actions/help.png").pixmap(pixmapSize);
      }
    }
  } else if (role != Qt::DisplayRole && role != Qt::EditRole)
    return QVariant();
  else
    return data;

  return QVariant();
}

QModelIndex RepeatedProtoModel::parent(const QModelIndex & /*index*/) const { return QModelIndex(); }

QVariant RepeatedProtoModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (this->empty() || role != Qt::DisplayRole || orientation != Qt::Orientation::Horizontal) return QVariant();
  ProtoModel *m = static_cast<ProtoModel *>(QObject::parent())->GetSubModel(field->number(), 0);
  return m->headerData(section, orientation, role);
}

QModelIndex RepeatedProtoModel::index(int row, int column, const QModelIndex & /*parent*/) const {
  return createIndex(row, column);
}

Qt::ItemFlags RepeatedProtoModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return nullptr;
  return QAbstractItemModel::flags(index);
}
