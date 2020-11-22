#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "Components/ArtManager.h"
#include "Models/ResourceModelMap.h"
#include "treenode.pb.h"

#include <QAbstractItemModel>
#include <QHash>

#include <unordered_map>

using TypeCase = buffers::TreeNode::TypeCase;
using IconMap = std::unordered_map<TypeCase, QIcon>;

class TreeModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  static IconMap iconMap;
  static enum UserRoles {
    TypeCaseRole = Qt::UserRole
  } useroles;

  explicit TreeModel(buffers::TreeNode *root, ResourceModelMap *resourceMap, QObject *parent);

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  // Retrieves the parent of the given node, or else returns nullptr.
  // This is required because the TreeModel uses raw TreeNode protos as its data
  // storage format, which cannot point to one another cyclically.
  buffers::TreeNode *Parent(buffers::TreeNode *node) const;
  // Builds the relative path of the given resource.
  QString MakeResourcePath(buffers::TreeNode *resource) const;

  Qt::DropActions supportedDropActions() const override;
  QStringList mimeTypes() const override;
  QMimeData *mimeData(const QModelIndexList &indexes) const override;
  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                    const QModelIndex &parent) override;

  QModelIndex insert(const QModelIndex &parent, int row, buffers::TreeNode *node);
  QModelIndex addNode(buffers::TreeNode *child, const QModelIndex &parent);
  buffers::TreeNode *duplicateNode(const buffers::TreeNode &node);
  void removeNode(const QModelIndex &index);
  void sortByName(const QModelIndex &index);

 signals:
  // Called when the name of a single TreeNode changes.
  void ResourceRenamed(TypeCase type, const QString &oldName, const QString &newName);
  // Called when a resource (or group of resources) is moved.
  void ResourceMoved(TreeNode *node, TreeNode *old_parent);

 private:
  buffers::TreeNode *root;
  ResourceModelMap *resourceMap;
  QHash<buffers::TreeNode *, buffers::TreeNode *> parents;

  void SetupParents(buffers::TreeNode *root);
  inline QString treeNodeMime() const { return QStringLiteral("RadialGM/TreeNode"); }
};

#endif  // TREEMODEL_H
