#ifndef MODELMAP_H
#define MODELMAP_H

#include "ProtoModel.h"

#include <QHash>
#include <QVector>
#include <string>

class ResourceModelMap : public QObject {
  Q_OBJECT
public:
  ResourceModelMap(buffers::TreeNode *root, QObject* parent);
  ProtoModel* GetResourceByName(int type, const QString& name);
  ProtoModel* GetResourceByName(int type, const std::string& name);

public slots:
  void ResourceRenamed(TypeCase type, const QString& oldName, const QString& newName);

protected:
  void recursiveBindRes(buffers::TreeNode *node, QObject* parent);
  QHash<int, QHash<QString, ProtoModel*>> _resources;
};

#endif // MODELMAP_H
