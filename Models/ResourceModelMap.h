#ifndef MODELMAP_H
#define MODELMAP_H

#include "MessageModel.h"

#include <QHash>
#include <QVector>
#include <string>

class ResourceModelMap : public QObject {
  Q_OBJECT
 public:
  ResourceModelMap(buffers::TreeNode* root, QObject* parent);
  MessageModel* GetResourceByName(int type, const QString& name);
  MessageModel* GetResourceByName(int type, const std::string& name);
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
  QHash<int, QHash<QString, QPair<buffers::TreeNode*, MessageModel*>>> _resources;
};

MessageModel* GetObjectSprite(const std::string& objName);
MessageModel* GetObjectSprite(const QString& objName);

#endif  // MODELMAP_H
