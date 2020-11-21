#include "Models/ResourceModelMap.h"
#include "Editors/BaseEditor.h"
#include "MainWindow.h"
#include "Models/RepeatedMessageModel.h"

ResourceModelMap::ResourceModelMap(buffers::TreeNode* root, QObject* parent) : QObject(parent) {
  recursiveBindRes(root);
}

void ResourceModelMap::recursiveBindRes(buffers::TreeNode* node) {
  for (int i = 0; i < node->child_size(); ++i) {
    buffers::TreeNode* child = node->mutable_child(i);
    if (child->folder()) {
      recursiveBindRes(child);
    }

    this->AddResource(child);
  }
}

void ResourceModelMap::AddResource(buffers::TreeNode* child) {
  MessageModel* model = child->has_folder() ? nullptr : new MessageModel(this, child);
  _resources[child->type_case()][QString::fromStdString(child->name())] =
      QPair<buffers::TreeNode*, MessageModel*>(child, model);

  if (model != nullptr) {
    connect(model, &ProtoModel::DataChanged, [this]() { emit DataChanged(); });
  }

  if (!child->has_folder()) {
    Resource res = {QString::fromStdString(child->name()), child, model};
    MainWindow::ResourceChanged(res, ResChange::Added);
  }
}

void ResourceModelMap::RemoveResource(TypeCase type, const QString& name) {
  if (!_resources.contains(type)) return;
  if (!_resources[type].contains(name)) return;

  Resource res = {name, _resources[type][name].first, _resources[type][name].second};
  MainWindow::ResourceChanged(res, ResChange::Removed);

  // Delete all instances of this object type
  if (type == TypeCase::kObject) {
    for (auto room : _resources[TypeCase::kRoom]) {
      MessageModel* roomModel = room.second->GetSubModel<MessageModel*>(TreeNode::kRoomFieldNumber);
      RepeatedMessageModel* instancesModel = roomModel->GetSubModel<RepeatedMessageModel*>(Room::kInstancesFieldNumber);
      RepeatedMessageModel::RowRemovalOperation remover(instancesModel);

      for (int row = 0; row < instancesModel->rowCount(); ++row) {
        if (instancesModel->Data(row, Room::Instance::kObjectTypeFieldNumber).toString() == name)
          remover.RemoveRow(row);
      }

      // Only models in use in open editors should have backup models
      MessageModel* backupModel = roomModel->GetBackupModel();
      if (backupModel != nullptr) {
        RepeatedMessageModel* instancesModelBak =
            backupModel->GetSubModel<RepeatedMessageModel*>(Room::kInstancesFieldNumber);
        RepeatedMessageModel::RowRemovalOperation removerBak(instancesModelBak);

        for (int row = 0; row < instancesModelBak->rowCount(); ++row) {
          if (instancesModelBak->Data(row, Room::Instance::kObjectTypeFieldNumber).toString() == name)
            removerBak.RemoveRow(row);
        }
      }
    }
  }

  // Delete all tiles using this background
  if (type == TypeCase::kBackground) {
    for (auto room : _resources[TypeCase::kRoom]) {
      MessageModel* roomModel = room.second->GetSubModel<MessageModel*>(TreeNode::kRoomFieldNumber);
      RepeatedMessageModel* tilesModel = roomModel->GetSubModel<RepeatedMessageModel*>(Room::kTilesFieldNumber);
      RepeatedMessageModel::RowRemovalOperation remover(tilesModel);

      for (int row = 0; row < tilesModel->rowCount(); ++row) {
        if (tilesModel->Data(row, Room::Instance::kObjectTypeFieldNumber).toString() == name) remover.RemoveRow(row);
      }

      // Only models in use in open editors should have backup models
      MessageModel* backupModel = roomModel->GetBackupModel();
      if (backupModel != nullptr) {
        RepeatedMessageModel* tilesModelBak = backupModel->GetSubModel<RepeatedMessageModel*>(Room::kTilesFieldNumber);
        RepeatedMessageModel::RowRemovalOperation removerBak(tilesModelBak);

        for (int row = 0; row < tilesModelBak->rowCount(); ++row) {
          if (tilesModelBak->Data(row, Room::Tile::kBackgroundNameFieldNumber).toString() == name)
            removerBak.RemoveRow(row);
        }
      }
    }
  }

  // Remove an references to this resource
  for (auto res : _resources) {
    for (auto model : res) {
      UpdateReferences(model.second, ResTypeAsString(type), name, "");
    }
  }

  _resources[type].remove(name);
  emit DataChanged();
}

QString ResourceModelMap::CreateResourceName(TreeNode* node) {
  auto fieldNum = ResTypeFields[node->type_case()];
  const Descriptor* desc = node->GetDescriptor();
  const FieldDescriptor* field = desc->FindFieldByNumber(fieldNum);
  const QString fieldName = node->folder() ? "group" : QString::fromStdString(field->name());
  return CreateResourceName(node->type_case(), fieldName);
}

QString ResourceModelMap::CreateResourceName(int type, const QString& typeName) {
  const QString pre = typeName;
  QString name;
  int i = 0;
  do {
    name = pre + QString::number(i++);
  } while (_resources[type].contains(name));
  return name;
}

MessageModel* ResourceModelMap::GetResourceByName(int type, const QString& name) {
  if (_resources[type].contains(name))
    return _resources[type][name].second;
  else
    return nullptr;
}

MessageModel* ResourceModelMap::GetResourceByName(int type, const std::string& name) {
  return GetResourceByName(type, QString::fromStdString(name));
}

void ResourceModelMap::ResourceRenamed(TypeCase type, const QString& oldName, const QString& newName) {
  if (oldName == newName || !_resources[type].contains(oldName)) return;
  _resources[type][newName] = _resources[type][oldName];

  Resource res = {newName, _resources[type][newName].first, _resources[type][newName].second};
  MainWindow::ResourceChanged(res, ResChange::Renamed, oldName);

  for (auto res : _resources) {
    for (auto model : res) {
      UpdateReferences(model.second, ResTypeAsString(type), oldName, newName);
    }
  }

  _resources[type].remove(oldName);

  emit DataChanged();
}

bool ResourceModelMap::ValidResourceName(const QString& name) {
  if (name.isEmpty()) return false;
  for (auto res : _resources) {
    if (res.contains(name)) return false;
  }

  for (const QChar& c : name) {
    if (!c.isLetterOrNumber() && c != '_') return false;
  }

  return true;
}

MessageModel* GetObjectSprite(const std::string& objName) { return GetObjectSprite(QString::fromStdString(objName)); }

MessageModel* GetObjectSprite(const QString& objName) {
  MessageModel* obj = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, objName);
  if (!obj) return nullptr;
  obj = obj->GetSubModel<MessageModel*>(TreeNode::kObjectFieldNumber);
  if (!obj) return nullptr;
  const QString spriteName = obj->Data(Object::kSpriteNameFieldNumber).toString();
  MessageModel* spr = MainWindow::resourceMap->GetResourceByName(TreeNode::kSprite, spriteName);
  if (spr) return spr->GetSubModel<MessageModel*>(TreeNode::kSpriteFieldNumber);
  return nullptr;
}
