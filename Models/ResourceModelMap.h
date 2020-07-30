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
  const QPersistentModelIndex& GetResourceByName(int type, const QString& name);
  const QPersistentModelIndex& GetResourceByName(int type, const std::string& name);
  void AddResource(buffers::TreeNode* node);
  void RemoveResource(TypeCase type, const QString& name);
  QString CreateResourceName(TreeNode* node);
  QString CreateResourceName(int type, const QString& typeName);

 public slots:
  void ResourceRenamed(TypeCase type, const QString& oldName, const QString& newName);

 signals:
  void DataChanged();

 protected:
  void recursiveBindRes(buffers::TreeNode* node);
  QHash<int, QHash<QString, QPair<buffers::TreeNode*, const QPersistentModelIndex&>>> _resources;
};

const QPersistentModelIndex& GetObjectSprite(const std::string& objName);
const QPersistentModelIndex& GetObjectSprite(const QString& objName);

#endif  // MODELMAP_H
