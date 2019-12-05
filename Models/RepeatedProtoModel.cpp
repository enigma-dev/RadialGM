#include "RepeatedProtoModel.h"
#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "MainWindow.h"
#include "ProtoModel.h"
#include "ResourceModelMap.h"

#include <QDebug>

RepeatedProtoModel::RepeatedProtoModel(Message *protobuf, const FieldDescriptor *field, ProtoModelBarePtr parent)
    : QAbstractItemModel(parent), parentModel(parent), protobuf(protobuf), field(field) {
  const Reflection *refl = protobuf->GetReflection();
  for (int j = 0; j < refl->FieldSize(*protobuf, field); j++) {
    AddModel(ProtoModelPtr(new ProtoModel(refl->MutableRepeatedMessage(protobuf, field, j), GetParentModel())));
  }
}

ProtoModelBarePtr RepeatedProtoModel::GetParentModel() const {
  return parentModel;
}

int RepeatedProtoModel::rowCount(const QModelIndex & /*parent*/) const {
  const Reflection *refl = protobuf->GetReflection();
  return refl->FieldSize(*protobuf, field);
}

void RepeatedProtoModel::AddModel(ProtoModelPtr model) {
  models.append(model);
}

ProtoModelPtr RepeatedProtoModel::GetSubModel(int index) {
  if (index < models.count())
    return models[index];
  else
    return nullptr;
}

QVector<ProtoModelPtr>& RepeatedProtoModel::GetMutableModelList() {
  return models;
}

bool RepeatedProtoModel::empty() const { return this->rowCount() <= 0; }

int RepeatedProtoModel::columnCount(const QModelIndex & /*parent*/) const {
  const Descriptor *desc = protobuf->GetDescriptor();
  return desc->field_count()+1;
}

//bool RepeatedProtoModel::setData(const QModelIndex &index, const QVariant &value, int role) {}

QVariant RepeatedProtoModel::data(int row, int column) const {
  return data(this->index(row, column, QModelIndex()), Qt::DisplayRole);
}

QVariant RepeatedProtoModel::data(const QModelIndex &index, int role) const {
  R_EXPECT(index.isValid(), QVariant()) << "Supplied index was invalid:" << index;

  if (index.column() == columnCount()-1 && role == Qt::DisplayRole) return index.row();

  ProtoModelPtr m = GetParentModel()->GetSubModel(field->number(), index.row());
  if (m == nullptr) return QVariant();
  QVariant data = m->data(index.column());
  if (role == Qt::DecorationRole && field->name() == "instances" &&
      index.column() == Room::Instance::kObjectTypeFieldNumber) {
    auto obj = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, data.toString());
    if (obj != nullptr) {
      obj = obj->GetSubModel(TreeNode::kObjectFieldNumber);
      ProtoModelPtr spr = MainWindow::resourceMap->GetResourceByName(
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
  ProtoModelPtr m = GetParentModel()->GetSubModel(field->number(), 0);
  return m->headerData(section, orientation, role);
}

QModelIndex RepeatedProtoModel::index(int row, int column, const QModelIndex & /*parent*/) const {
  return createIndex(row, column);
}

Qt::ItemFlags RepeatedProtoModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return nullptr;
  return QAbstractItemModel::flags(index);
}

void RepeatedProtoModel::RowRemovalOperation::RemoveRow(int row) { return RemoveRows(row, 1); }
void RepeatedProtoModel::RowRemovalOperation::RemoveRows(int row, int count) {
  auto list = model->GetMutableModelList();
  model->beginRemoveRows(QModelIndex(), row, row + count - 1);
  if (left < right) {
    while (right < row) {
      field.SwapElements(left, right);
      list[left].swap(list[right]);
      left++; right++;
    }
  } else {
    left = row;
  }
  right = row + count;
  model->endRemoveRows();
}

RepeatedProtoModel::RowRemovalOperation::~RowRemovalOperation() {
  auto &list = model->GetMutableModelList();
  qDebug() << left << "," << right << "," << field.size();
  if (left < right) {
    qDebug() << "swap final " << field.size() - right << " rows backward";
    while (right < field.size()) {
      field.SwapElements(left, right);
      list[left].swap(list[right]);
      left++; right++;
    }
    
    qDebug() << "remove final " << field.size() - left << " rows";
    while (left < field.size()) field.RemoveLast();
    list.resize(left);
  }
}
