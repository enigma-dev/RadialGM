#include "Models/ResourceModelMap.h"
#include "Editors/BaseEditor.h"
#include "MainWindow.h"

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

}

void ResourceModelMap::RemoveResource(TypeCase type, const QString& name) {

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

const QPersistentModelIndex& ResourceModelMap::GetResourceByName(int type, const QString& name) {

}

const QPersistentModelIndex& ResourceModelMap::GetResourceByName(int type, const std::string& name) {

}

void ResourceModelMap::ResourceRenamed(TypeCase type, const QString& oldName, const QString& newName) {

}

const QPersistentModelIndex& GetObjectSprite(const std::string& objName) {  }

const QPersistentModelIndex& GetObjectSprite(const QString& objName) {

}
