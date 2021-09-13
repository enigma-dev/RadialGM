#ifndef MODELMAP_H
#define MODELMAP_H

#include "MessageModel.h"
#include "TreeModel.h"

#include <QHash>
#include <QIcon>
#include <QVector>
#include <string>

class ResourceModelMap : public QObject {
  Q_OBJECT
 public:
  ResourceModelMap(QObject* parent);
  MessageModel* GetResourceByName(int type, const QString& name);
  MessageModel* GetResourceByName(int type, const std::string& name);
  void AddResource(TypeCase type, const QString& name, MessageModel* model);
  QString CreateResourceName(TreeNode* node);
  QString CreateResourceName(int type, const QString& typeName);
  bool ValidName(TypeCase type, const QString& name);

 public slots:
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
  void TreeChanged(MessageModel* model);
  void ResourceRenamed(TypeCase type, const QString& oldName, const QString& newName);
  void ResourceRemoved(TypeCase type, const QString& name,
                      std::map<ProtoModel*, RepeatedModel::RowRemovalOperation>& removers);

 signals:
  void DataChanged();
  void ResourceRenamed(const std::string& type, const QString& oldName, const QString& newName);

 protected:
  QHash<int, QHash<QString, MessageModel*>> _resources;
};

MessageModel* GetObjectSprite(const std::string& object_name);
MessageModel* GetObjectSprite(const QString& object_name);

QIcon GetSpriteIconByName(const QString& sprite_name);
/// Wraps GetSpriteIconByName, but expects the QVariant field content rather than a direct string.
QIcon GetSpriteIconByNameField(const QVariant& sprite_name);
/// Retrieves the icon for the sprite assigned to the object with the given name.
QIcon GetObjectSpriteByNameField(const QVariant& object_name);
/// Retrieves the icon for the given background.
QIcon GetBackgroundIconByNameField(const QVariant& bkg_name);
/// Retrieves the icon for the given file.
QIcon GetFileIcon(const QVariant& fname);

#endif  // MODELMAP_H
