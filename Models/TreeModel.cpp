#include "TreeModel.h"

#include "Components/ArtManager.h"
#include "Models/ResourceModelMap.h"

#include <unordered_map>

TreeModel::TreeModel(buffers::TreeNode *root, QObject *parent) : QAbstractItemModel(parent), root(root) {
  SetupParents(root);
}

void TreeModel::SetupParents(buffers::TreeNode *root) {
  for (int i = 0; i < root->child_size(); ++i) {
    auto child = root->mutable_child(i);
    parents[child] = root;
    SetupParents(child);
  }
}

int TreeModel::columnCount(const QModelIndex & /*parent*/) const { return 1; }

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (!index.isValid() || role != Qt::EditRole) return false;
  buffers::TreeNode *item = static_cast<buffers::TreeNode *>(index.internalPointer());
  item->set_name(value.toString().toStdString());
  emit dataChanged(index, index);
  return true;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
  using TypeCase = buffers::TreeNode::TypeCase;
  using IconMap = std::unordered_map<TypeCase, QIcon>;

  if (!index.isValid()) return QVariant();

  buffers::TreeNode *item = static_cast<buffers::TreeNode *>(index.internalPointer());
  if (role == Qt::DecorationRole) {
    static IconMap iconMap({{TypeCase::kFolder, ArtManager::GetIcon("group")},
                            {TypeCase::kSprite, ArtManager::GetIcon("sprite")},
                            {TypeCase::kSound, ArtManager::GetIcon("sound")},
                            {TypeCase::kBackground, ArtManager::GetIcon("background")},
                            {TypeCase::kPath, ArtManager::GetIcon("path")},
                            {TypeCase::kScript, ArtManager::GetIcon("script")},
                            {TypeCase::kShader, ArtManager::GetIcon("shader")},
                            {TypeCase::kFont, ArtManager::GetIcon("font")},
                            {TypeCase::kTimeline, ArtManager::GetIcon("timeline")},
                            {TypeCase::kObject, ArtManager::GetIcon("object")},
                            {TypeCase::kRoom, ArtManager::GetIcon("room")},
                            {TypeCase::kSettings, ArtManager::GetIcon("settings")}});

    auto it = iconMap.find(item->type_case());
    if (it == iconMap.end()) return ArtManager::GetIcon("info");

    if (item->type_case() == TypeCase::kSprite) {
      QString spr = QString::fromStdString(item->sprite().subimages(0));
      if (!spr.isEmpty()) return ArtManager::GetIcon(spr);
    }

    if (item->type_case() == TypeCase::kBackground) {
      QString bkg = QString::fromStdString(item->background().image());
      if (!bkg.isEmpty()) return ArtManager::GetIcon(bkg);
    }

    if (item->type_case() == TypeCase::kObject) {
      const ProtoModel* sprModel = GetObjectSprite(item->name());
      if (sprModel != nullptr) {
        QString spr = sprModel->GetString(Sprite::kSubimagesFieldNumber, 0);
        if (!spr.isEmpty()) return ArtManager::GetIcon(spr);
      }
    }

    const QIcon &icon = it->second;
    if (item->type_case() == TypeCase::kFolder && item->child_size() <= 0) {
      return icon.pixmap(icon.availableSizes().first(), QIcon::Disabled);
    }
    return icon;
  } else if (role == Qt::EditRole || role == Qt::DisplayRole) {
    return QString::fromStdString(item->name());
  }

  return QVariant();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return nullptr;

  return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QVariant TreeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  return tr("Name");
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent)) return QModelIndex();

  buffers::TreeNode *parentItem;

  if (!parent.isValid())
    parentItem = root;
  else
    parentItem = static_cast<buffers::TreeNode *>(parent.internalPointer());

  buffers::TreeNode *childItem = parentItem->mutable_child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
  if (!index.isValid()) return QModelIndex();

  buffers::TreeNode *childItem = static_cast<buffers::TreeNode *>(index.internalPointer());
  buffers::TreeNode *parentItem = parents[childItem];

  if (parentItem == root) return QModelIndex();

  return createIndex(parentItem->child_size(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const {
  buffers::TreeNode *parentItem;
  if (parent.column() > 0) return 0;

  if (!parent.isValid())
    parentItem = root;
  else
    parentItem = static_cast<buffers::TreeNode *>(parent.internalPointer());

  return parentItem->child_size();
}
