#include "RepeatedProtoModel.h"
#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "MainWindow.h"
#include "ProtoModel.h"
#include "ResourceModelMap.h"

RepeatedProtoModel::RepeatedProtoModel(Message *protobuf, const FieldDescriptor *field, ProtoModelPtr parent)
    : QAbstractItemModel(parent), parentModel(parent), protobuf(protobuf), field(field) {
  const Reflection *refl = protobuf->GetReflection();
  for (int j = 0; j < refl->FieldSize(*protobuf, field); j++) {
    AddModel(ProtoModelPtr(new ProtoModel(refl->MutableRepeatedMessage(protobuf, field, j), GetParentModel())));
  }
}

ProtoModelPtr RepeatedProtoModel::GetParentModel() const {
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
  return desc->field_count();
}

//bool RepeatedProtoModel::setData(const QModelIndex &index, const QVariant &value, int role) {}

QVariant RepeatedProtoModel::data(int row, int column) const {
  return data(this->index(row, column, QModelIndex()), Qt::DisplayRole);
}

QVariant RepeatedProtoModel::data(const QModelIndex &index, int role) const {
  R_EXPECT(index.isValid(), QVariant()) << "Supplied index was invalid:" << index;

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
  } else if (role == Qt::DecorationRole && field->name() == "tiles" &&
      index.column() == Room::Tile::kBackgroundNameFieldNumber) {
    auto bkg = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, data.toString());
    if (bkg != nullptr) {
      bkg = bkg->GetSubModel(TreeNode::kBackgroundFieldNumber);
      static const QSize pixmapSize(18, 18);
      return ArtManager::GetIcon(bkg->data(Background::kImageFieldNumber).toString()).pixmap(pixmapSize);
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

void RepeatedProtoModel::RowRemovalOperation::RemoveRow(int row) {
  rows.insert(row);
}
void RepeatedProtoModel::RowRemovalOperation::RemoveRows(int row, int count) {
  for (int i = row; i < row + count; ++i) rows.insert(i);
}

RepeatedProtoModel::RowRemovalOperation::~RowRemovalOperation() {
  auto &list = model->GetMutableModelList();
  if (rows.empty()) return;

  // Compute ranges for our deleted rows.
  struct Range {
    int first, last;
    Range(): first(), last() {}
    Range(int f, int l): first(f), last(l) {}
    int size() { return last - first + 1; }
  };
  std::vector<Range> ranges;
  for (int row : rows) {
    if (ranges.empty() || row != ranges.back().last + 1) {
      ranges.emplace_back(row, row);
    } else {
      ranges.back().last = row;
    }
  }

  // Broadcast range removal before the model can fuck anything up.
  // Do this from back to front to minimize the amount of shit it fucks up.
  for (auto range = ranges.rbegin(); range != ranges.rend(); ++range) {
    model->beginRemoveRows(QModelIndex(), range->first, range->last);
  }

  // Basic dense range removal. Move "deleted" rows to the end of the array.
  int left = 0, right = 0;
  for (auto range : ranges) {
    while (right < range.first) {
      field.SwapElements(left, right);
      std::swap(list[left], list[right]);
      left++; right++;
    }
    right = range.last + 1;
  }
  while (right < field.size()) {
    field.SwapElements(left, right);
    std::swap(list[left], list[right]);
    left++; right++;
  }

  // Send the endRemoveRows operations in the reverse order, removing the
  // correct number of rows incrementally, or else various components in Qt
  // will bitch, piss, moan, wail, whine, and cry. Actually, they will anyway.
  for (Range range : ranges) {
    list.resize(list.size() - range.size());
    for (int j = range.first; j <= range.last; ++j) field.RemoveLast();
    model->endRemoveRows();
  }
}
