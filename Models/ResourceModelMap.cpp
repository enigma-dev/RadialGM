#include "Models/ResourceModelMap.h"

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
  return _resources[type][name];
}

ProtoModel* ResourceModelMap::GetResourceByName(int type, const std::string& name) {
  return GetResourceByName(type, QString::fromStdString(name));
}
