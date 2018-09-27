#ifndef MODELMAP_H
#define MODELMAP_H

#include "ProtoModel.h"

#include "codegen/treenode.pb.h"

#include <QHash>
#include <QVector>
#include <string>

class ResourceModelMap {
public:
  ResourceModelMap(buffers::TreeNode *root);
  ProtoModel* GetResourceByName(int type, const QString& name);
  ProtoModel* GetResourceByName(int type, const std::string& name);
protected:
  void recursiveBindRes(buffers::TreeNode *node);
  QHash<int, QHash<QString, ProtoModel*>> _resources;
};

#endif // MODELMAP_H
