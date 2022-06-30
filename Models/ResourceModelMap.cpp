#include "Models/ResourceModelMap.h"
#include "Editors/BaseEditor.h"
#include "MainWindow.h"
#include "Models/RepeatedMessageModel.h"

static std::string ResTypeAsString(TypeCase type) {
  switch (type) {
    case TypeCase::kFolder: return "treenode";
    case TypeCase::kBackground: return "background";
    case TypeCase::kFont: return "font";
    case TypeCase::kObject: return "object";
    case TypeCase::kPath: return "path";
    case TypeCase::kRoom: return "room";
    case TypeCase::kSound: return "sound";
    case TypeCase::kSprite: return "sprite";
    case TypeCase::kShader: return "shader";
    case TypeCase::kScript: return "script";
    case TypeCase::kSettings: return "settings";
    case TypeCase::kInclude: return "include";
    case TypeCase::kTimeline: return "timeline";
    case TypeCase::TYPE_NOT_SET:
    case TypeCase::kUnknown: return "unknown";
  }
  return "unknown";
}

ResourceModelMap::ResourceModelMap(QObject* parent) : QObject(parent) {}

static void TreeChangedHelper(MessageModel* model, ResourceModelMap* rm) {
  const MessageModel* folder = model->GetSubModel<MessageModel*>(TreeNode::kFolderFieldNumber);
  if (folder) {
    const RepeatedMessageModel* children =
        folder->GetSubModel<RepeatedMessageModel*>(TreeNode::Folder::kChildrenFieldNumber);
    if (children) {
      for (int i = 0; i < children->rowCount(); ++i) {
        TreeChangedHelper(children->GetSubModel(i)->TryCastAsMessageModel(), rm);
      }
    }
  } else {
    int type = model->OneOfType("type");
    rm->AddResource((buffers::TreeNode::TypeCase)type,
                    model->Data(FieldPath::Of<TreeNode>(TreeNode::kNameFieldNumber)).toString(), model);
  }
}

void ResourceModelMap::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) {
  emit DataChanged();
}

void ResourceModelMap::TreeChanged(MessageModel* model) {
  _resources.clear();
  TreeChangedHelper(model, this);
}

void ResourceModelMap::AddResource(TypeCase type, const QString& name, MessageModel* model) {
  R_EXPECT_V(!_resources[type].contains(name))
      << "Resource" << ResTypeAsString(type) << "with name:" << name << "already exists";
  _resources[type][name] = model;
}

void ResourceModelMap::ResourceRemoved(TypeCase type, const QString& name,
                                      std::map<ProtoModel*, RepeatedMessageModel::RowRemovalOperation>& removers) {
  if (type == TypeCase::kFolder || !_resources.contains(type)) return;
  if (!_resources[type].contains(name)) return;

  // Delete all instances of this object type
  if (type == TypeCase::kObject) {
    for (auto& room : qAsConst(_resources[TypeCase::kRoom])) {
      R_EXPECT_V(room);
      MessageModel* roomModel = room->GetSubModel<MessageModel*>(TreeNode::kRoomFieldNumber);
      R_EXPECT_V(roomModel);
      RepeatedMessageModel* instancesModel = roomModel->GetSubModel<RepeatedMessageModel*>(Room::kInstancesFieldNumber);
      auto& remover = removers.emplace(instancesModel, instancesModel).first->second;

      for (int row = 0; row < instancesModel->rowCount(); ++row) {
        if (instancesModel
                ->Data(
                    FieldPath::Of<Room::Instance>(FieldPath::StartingAt(row), Room::Instance::kObjectTypeFieldNumber))
                .toString() == name)
          remover.RemoveRow(row);
      }

      // Only models in use in open editors should have backup models
      MessageModel* backupModel = roomModel->GetBackupModel();
      if (backupModel != nullptr) {
        RepeatedMessageModel* instancesModelBak =
            backupModel->GetSubModel<RepeatedMessageModel*>(Room::kInstancesFieldNumber);
        auto& remover = removers.emplace(instancesModelBak, instancesModelBak).first->second;

        for (int row = 0; row < instancesModelBak->rowCount(); ++row) {
          if (instancesModelBak
                  ->Data(
                      FieldPath::Of<Room::Instance>(FieldPath::StartingAt(row), Room::Instance::kObjectTypeFieldNumber))
                  .toString() == name)
            remover.RemoveRow(row);
        }
      }
    }
  }

  // Delete all tiles using this background
  if (type == TypeCase::kBackground) {
    for (auto& room : qAsConst(_resources[TypeCase::kRoom])) {
      MessageModel* roomModel = room->GetSubModel<MessageModel*>(TreeNode::kRoomFieldNumber);
      RepeatedMessageModel* tilesModel = roomModel->GetSubModel<RepeatedMessageModel*>(Room::kTilesFieldNumber);
      auto& remover = removers.emplace(tilesModel, tilesModel).first->second;

      for (int row = 0; row < tilesModel->rowCount(); ++row) {
        if (tilesModel
                ->Data(
                    FieldPath::Of<Room::Instance>(FieldPath::StartingAt(row), Room::Instance::kObjectTypeFieldNumber))
                .toString() == name)
          remover.RemoveRow(row);
      }

      // Only models in use in open editors should have backup models
      MessageModel* backupModel = roomModel->GetBackupModel();
      if (backupModel != nullptr) {
        RepeatedMessageModel* tilesModelBak = backupModel->GetSubModel<RepeatedMessageModel*>(Room::kTilesFieldNumber);
        auto& remover = removers.emplace(tilesModelBak, tilesModelBak).first->second;

        for (int row = 0; row < tilesModelBak->rowCount(); ++row) {
          if (tilesModelBak
                  ->Data(FieldPath::Of<Room::Tile>(FieldPath::StartingAt(row), Room::Tile::kBackgroundNameFieldNumber))
                  .toString() == name)
            remover.RemoveRow(row);
        }
      }
    }
  }

  // NOTE: There is an unhandled BUG related to order in which references are deleted,
  // and following hack doesnt solve it

  // Remove an references to this resource
  //emit ResourceRenamed(ResTypeAsString(type), name, "");

  // Remove references to this resource
  _resources[type].remove(name);
  emit DataChanged();
}

QString ResourceModelMap::CreateResourceName(TreeNode* node) {
  auto fieldNum = ResTypeFields[node->type_case()];
  const Descriptor* desc = node->GetDescriptor();
  const FieldDescriptor* field = desc->FindFieldByNumber(fieldNum);
  const QString fieldName = node->has_folder() ? "group" : QString::fromStdString(field->name());
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

bool ResourceModelMap::ValidName(TypeCase type, const QString& name) {
  if (name.isEmpty() || !name[0].isLetter()) return false;
  for (const auto& c : name) {
    if (!c.isLetterOrNumber() && c != '_') return false;
  }
  return !_resources[type].contains(name);
}

MessageModel* ResourceModelMap::GetResourceByName(int type, const QString& name) {
  if (_resources[type].contains(name))
    return _resources[type][name];
  else
    return nullptr;
}

MessageModel* ResourceModelMap::GetResourceByName(int type, const std::string& name) {
  return GetResourceByName(type, QString::fromStdString(name));
}

template <typename Message>
const std::string& FullName() {
  return Message::descriptor()->full_name();
}

TypeCase Type(TreeModel::Node* node) {
  static const std::unordered_map<std::string, TypeCase> kTypesByMessage{
      {FullName<buffers::resources::Sprite>(), TypeCase::kSprite},
      {FullName<buffers::resources::Sound>(), TypeCase::kSound},
      {FullName<buffers::resources::Background>(), TypeCase::kBackground},
      {FullName<buffers::resources::Path>(), TypeCase::kPath},
      {FullName<buffers::resources::Font>(), TypeCase::kFont},
      {FullName<buffers::resources::Script>(), TypeCase::kScript},
      {FullName<buffers::resources::Shader>(), TypeCase::kShader},
      {FullName<buffers::resources::Timeline>(), TypeCase::kTimeline},
      {FullName<buffers::resources::Object>(), TypeCase::kObject},
      {FullName<buffers::resources::Room>(), TypeCase::kRoom},
      {FullName<buffers::resources::Settings>(), TypeCase::kSettings},
  };

  auto res = kTypesByMessage.find(node->GetMessageType());
  if (res == kTypesByMessage.end()) return TypeCase::TYPE_NOT_SET;
  return res->second;
}

void ResourceModelMap::ResourceRenamed(TypeCase type, const QString& oldName, const QString& newName) {
  if (oldName == newName || !_resources[type].contains(oldName)) return;
  _resources[type][newName] = _resources[type][oldName];

  emit ResourceRenamed(ResTypeAsString(type), oldName, newName);
  _resources[type].remove(oldName);

  emit DataChanged();
}

MessageModel* GetObjectSprite(const std::string& object_name) {
  return GetObjectSprite(QString::fromStdString(object_name));
}

MessageModel* GetObjectSprite(const QString& object_name) {
  MessageModel* obj = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, object_name);
  if (!obj) return nullptr;
  obj = obj->GetSubModel<MessageModel*>(TreeNode::kObjectFieldNumber);
  if (!obj) return nullptr;
  const QString spriteName = obj->Data(FieldPath::Of<Object>(Object::kSpriteNameFieldNumber)).toString();
  MessageModel* spr = MainWindow::resourceMap->GetResourceByName(TreeNode::kSprite, spriteName);
  if (spr) return spr->GetSubModel<MessageModel*>(TreeNode::kSpriteFieldNumber);
  return nullptr;
}

QIcon GetSpriteIconByNameField(const QVariant& sprite_name) {
  if (sprite_name.isNull()) return {};
  return GetSpriteIconByName(sprite_name.toString());
}

QIcon GetSpriteIconByName(const QString& sprite_name) {
  ProtoModel* spr = MainWindow::resourceMap->GetResourceByName(TreeNode::kSprite, sprite_name);
  if (!spr) return {};
  spr = spr->GetSubModel(FieldPath::Of<TreeNode>(TreeNode::kSpriteFieldNumber));
  auto subimgs = spr->GetSubModel(FieldPath::Of<Sprite>(Sprite::kSubimagesFieldNumber));
  if (subimgs->rowCount() == 0) return {};
  QVariant path = subimgs->DataAtRow(0);
  if (path.isNull()) return {};
  return ArtManager::GetIcon(path.toString());
}

QIcon GetObjectSpriteByNameField(const QVariant& object_name) {
  std::string object_name_str = object_name.toString().toStdString();
  MessageModel* obj = MainWindow::resourceMap->GetResourceByName(TreeNode::kObject, object_name_str);
  if (!obj) return QIcon();
  obj = obj->GetSubModel<MessageModel*>(TreeNode::kObjectFieldNumber);
  if (!obj) return QIcon();
  return GetSpriteIconByNameField(obj->Data(FieldPath::Of<Object>(Object::kSpriteNameFieldNumber)));
}

QIcon GetBackgroundIconByNameField(const QVariant& bkg_name) {
  std::string bkg_name_str = bkg_name.toString().toStdString();
  MessageModel* bkg = MainWindow::resourceMap->GetResourceByName(TreeNode::kBackground, bkg_name_str);
  if (!bkg) return QIcon();
  bkg = bkg->GetSubModel<MessageModel*>(TreeNode::kBackgroundFieldNumber);
  if (!bkg) return QIcon();
  return ArtManager::GetIcon(bkg->Data(FieldPath::Of<Background>(Background::kImageFieldNumber)).toString());
}

QIcon GetFileIcon(const QVariant& fname) { return ArtManager::GetIcon(fname.toString()); }
