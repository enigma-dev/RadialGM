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
  ProtoModelPtr GetResourceByName(int type, const QString& name);
  ProtoModelPtr GetResourceByName(int type, const std::string& name);
  void AddResource(buffers::TreeNode* node, QObject* parent);
  void RemoveResource(TypeCase type, const QString& name);
  QString CreateResourceName(TreeNode* node);
  QString CreateResourceName(int type, const QString& typeName);

 public slots:
  void ResourceRenamed(TypeCase type, const QString& oldName, const QString& newName);

 signals:
  void dataChanged();

 protected:
  void recursiveBindRes(buffers::TreeNode* node, QObject* parent);
  QHash<int, QHash<QString, QPair<buffers::TreeNode*, ProtoModelPtr>>> _resources;
};

const ProtoModelPtr GetObjectSprite(const std::string& objName);
const ProtoModelPtr GetObjectSprite(const QString& objName);

#endif  // MODELMAP_H
