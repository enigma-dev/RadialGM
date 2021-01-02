#ifndef MODELMAP_H
#define MODELMAP_H

#include "MessageModel.h"
#include "TreeModel.h"

#include <QHash>
#include <QVector>
#include <QIcon>
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
  void ResourceRenamed(TreeModel::Node *node, const QString& oldName, const QString& newName);

 signals:
  void DataChanged();

 protected:
  void recursiveBindRes(buffers::TreeNode* node);
  QHash<int, QHash<QString, QPair<buffers::TreeNode*, MessageModel*>>> _resources;
};

MessageModel* GetObjectSprite(const std::string& objName);
MessageModel* GetObjectSprite(const QString& objName);
QIcon GetSpriteIconByName(const QString& sprite_name);
/// Wraps GetSpriteIconByName, but expects the QVariant field content rather than a direct string.
QIcon GetSpriteIconByNameField(const QVariant& sprite_name);

#endif  // MODELMAP_H
