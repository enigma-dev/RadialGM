#include "Models/ResourceModelMap.h"
#include "Editors/BaseEditor.h"
#include "MainWindow.h"

ResourceModelMap::ResourceModelMap(buffers::TreeNode* root, QObject* parent) : QObject(parent) {
  recursiveBindRes(root, this);
}

void ResourceModelMap::recursiveBindRes(buffers::TreeNode* node, QObject* parent) {
  for (int i = 0; i < node->child_size(); ++i) {
    buffers::TreeNode* child = node->mutable_child(i);
    if (child->folder()) {
      recursiveBindRes(child, parent);
    }

    this->AddResource(child, parent);
  }
}

void ResourceModelMap::AddResource(buffers::TreeNode* child, QObject* parent) {
  _resources[child->type_case()][QString::fromStdString(child->name())] =
      QPair<buffers::TreeNode*, ProtoModelPtr>(child, child->has_folder() ? nullptr : ProtoModelPtr(new ProtoModel(child, this)));
}

void ResourceModelMap::RemoveResource(TypeCase type, const QString& name) {
  if (!_resources.contains(type)) return;
  if (!_resources[type].contains(name)) return;

  // Delete all instances of this object type
  if (type == TypeCase::kObject) {
    for (auto room : _resources[TypeCase::kRoom]) {
      ProtoModelPtr roomModel = room.second->GetSubModel(TreeNode::kRoomFieldNumber);
      RepeatedProtoModelPtr instancesModel = roomModel->GetRepeatedSubModel(Room::kInstancesFieldNumber);
      RepeatedProtoModel::RowRemovalOperation remover(instancesModel);

      for (int row = 0; row < instancesModel->rowCount(); ++row) {
        if (instancesModel->data(row, Room::Instance::kObjectTypeFieldNumber).toString() == name)
          remover.RemoveRow(row);
      }

      // Only models in use in open editors should have backup models
      ProtoModelPtr backupModel = roomModel->GetBackupModel();
      if (backupModel != nullptr) {
        RepeatedProtoModelPtr instancesModelBak = backupModel->GetRepeatedSubModel(Room::kInstancesFieldNumber);
        RepeatedProtoModel::RowRemovalOperation removerBak(instancesModelBak);

        for (int row = 0; row < instancesModelBak->rowCount(); ++row) {
          if (instancesModelBak->data(row, Room::Instance::kObjectTypeFieldNumber).toString() == name)
            removerBak.RemoveRow(row);
        }
      }
    }
  }

  // Delete all tiles using this background
  if (type == TypeCase::kBackground) {
    for (auto room : _resources[TypeCase::kRoom]) {
      ProtoModelPtr roomModel = room.second->GetSubModel(TreeNode::kRoomFieldNumber);
      RepeatedProtoModelPtr tilesModel = roomModel->GetRepeatedSubModel(Room::kTilesFieldNumber);
      RepeatedProtoModel::RowRemovalOperation remover(tilesModel);

      for (int row = 0; row < tilesModel->rowCount(); ++row) {
        if (tilesModel->data(row, Room::Instance::kObjectTypeFieldNumber).toString() == name)
          remover.RemoveRow(row);
      }

      // Only models in use in open editors should have backup models
      ProtoModelPtr backupModel = roomModel->GetBackupModel();
      if (backupModel != nullptr) {
        RepeatedProtoModelPtr tilesModelBak = backupModel->GetRepeatedSubModel(Room::kTilesFieldNumber);
        RepeatedProtoModel::RowRemovalOperation removerBak(tilesModelBak);

        for (int row = 0; row < tilesModelBak->rowCount(); ++row) {
          if (tilesModelBak->data(row, Room::Tile::kBackgroundNameFieldNumber).toString() == name)
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

ProtoModelPtr ResourceModelMap::GetResourceByName(int type, const QString& name) {
  if (_resources[type].contains(name))
    return _resources[type][name].second;
  else
    return nullptr;
}

ProtoModelPtr ResourceModelMap::GetResourceByName(int type, const std::string& name) {
  return GetResourceByName(type, QString::fromStdString(name));
}


void ResourceModelMap::ResourceRenamed(TypeCase type, const QString& oldName, const QString& newName) {
  if (oldName == newName || !_resources[type].contains(oldName)) return;
  _resources[type][newName] = _resources[type][oldName];
  
  for (auto res : _resources) {
    for (auto model : res) {
      UpdateReferences(model.second, ResTypeAsString(type), oldName, newName);
    }
  }
  
  _resources[type].remove(oldName);
}

const ProtoModelPtr GetObjectSprite(const std::string& objName) {
  return GetObjectSprite(QString::fromStdString(objName));
}

const ProtoModelPtr GetObjectSprite(const QString& objName) {
  ProtoModelPtr obj = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, objName);
  if (!obj) return nullptr;
  obj = obj->GetSubModel(TreeNode::kObjectFieldNumber);
  if (!obj) return nullptr;
  const QString spriteName = obj->data(Object::kSpriteNameFieldNumber).toString();
  ProtoModelPtr spr = MainWindow::resourceMap->GetResourceByName(TreeNode::kSprite, spriteName);
  if (spr) return spr->GetSubModel(TreeNode::kSpriteFieldNumber);
  return nullptr;
}
