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

 public slots:
  void ResourceRenamed(TypeCase type, const QString& oldName, const QString& newName);

 protected:
  void recursiveBindRes(buffers::TreeNode* node, QObject* parent);
  QHash<int, QHash<QString, ProtoModel*>> _resources;
};

const ProtoModel* GetObjectSprite(const std::string& objName);
const ProtoModel* GetObjectSprite(const QString& objName);

#endif  // MODELMAP_H
