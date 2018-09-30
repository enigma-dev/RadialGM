#include "Models/ResourceModelMap.h"

using TypeCase = buffers::TreeNode::TypeCase;
QHash<int, int> ResourceModelMap::_resTypeFields = {
    { TypeCase::kSprite,     buffers::TreeNode::kSpriteFieldNumber     },
    { TypeCase::kSound,      buffers::TreeNode::kSoundFieldNumber      },
    { TypeCase::kBackground, buffers::TreeNode::kBackgroundFieldNumber },
    { TypeCase::kPath,       buffers::TreeNode::kPathFieldNumber       },
    { TypeCase::kFont,       buffers::TreeNode::kFontFieldNumber       },
    { TypeCase::kTimeline,   buffers::TreeNode::kTimelineFieldNumber   },
    { TypeCase::kObject,     buffers::TreeNode::kObjectFieldNumber     },
    { TypeCase::kRoom,       buffers::TreeNode::kRoomFieldNumber       }
};

ResourceModelMap::ResourceModelMap(buffers::TreeNode *root) {
  recursiveBindRes(root);
}

void ResourceModelMap::recursiveBindRes(buffers::TreeNode *node) {

    for (int i=0; i < node->child_size(); ++i) {
      buffers::TreeNode* child = node->mutable_child(i);
      if (child->folder()) {
        recursiveBindRes(child);
        continue;
      }

      _resources[child->type_case()][QString::fromStdString(child->name())] = new ProtoModel(child, nullptr);
    }
}

ProtoModel* ResourceModelMap::GetResourceByName(int type, const QString& name) {
  if (_resources[type].contains(name))
    return _resources[type][name]->GetSubModel(_resTypeFields[type]);
  else return nullptr;
}

ProtoModel* ResourceModelMap::GetResourceByName(int type, const std::string& name) {
  return GetResourceByName(type, QString::fromStdString(name));
}
