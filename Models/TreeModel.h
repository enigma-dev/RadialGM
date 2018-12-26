#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "Components/ArtManager.h"
#include "Models/ResourceModelMap.h"
#include "codegen/treenode.pb.h"

#include <QAbstractItemModel>
#include <QHash>

#include <unordered_map>

using TypeCase = buffers::TreeNode::TypeCase;
using IconMap = std::unordered_map<TypeCase, QIcon>;

class TreeModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  static IconMap iconMap;

  explicit TreeModel(buffers::TreeNode *root, ResourceModelMap *resourceMap, QObject *parent);

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  Qt::DropActions supportedDropActions() const override;
  QStringList mimeTypes() const override;
  QMimeData *mimeData(const QModelIndexList &indexes) const override;
  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                    const QModelIndex &parent) override;

  void insert(const QModelIndex &parent, int row, buffers::TreeNode *node);
  QModelIndex addNode(buffers::TreeNode *child, const QModelIndex &parent);
  buffers::TreeNode *duplicateNode(const buffers::TreeNode &node);
  void removeNode(const QModelIndex &index);
  void sortByName(const QModelIndex &index);

 signals:
  void ResourceRenamed(TypeCase type, const QString &oldName, const QString &newName);

 private:
  buffers::TreeNode *root;
  ResourceModelMap *resourceMap;
  QHash<buffers::TreeNode *, buffers::TreeNode *> parents;

  void SetupParents(buffers::TreeNode *root);
  inline QString treeNodeMime() const { return QStringLiteral("RadialGM/TreeNode"); }
};

#endif  // TREEMODEL_H
