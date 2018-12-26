#ifndef MODELMAP_H
#define MODELMAP_H

#include "ProtoModel.h"

#include <QHash>
#include <QVector>
#include <string>

class ResourceModelMap : public QObject {
  Q_OBJECT
 public:
  ResourceModelMap(buffers::TreeNode* root, QObject* parent);
  ProtoModel* GetResourceByName(int type, const QString& name);
  ProtoModel* GetResourceByName(int type, const std::string& name);
  void AddResource(buffers::TreeNode* node, QObject* parent);
  void RemoveResource(int type, const QString& name);
  QString CreateResourceName(TreeNode* node);
  QString CreateResourceName(int type, const QString& typeName);

 public slots:
  void ResourceRenamed(TypeCase type, const QString& oldName, const QString& newName);

 protected:
  void recursiveBindRes(buffers::TreeNode* node, QObject* parent);
  QHash<int, QHash<QString, QPair<buffers::TreeNode*, ProtoModel*>>> _resources;
};

const ProtoModel* GetObjectSprite(const std::string& objName);
const ProtoModel* GetObjectSprite(const QString& objName);

#endif  // MODELMAP_H
