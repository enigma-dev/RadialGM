#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include "treenode.pb.h"

#include <QAbstractItemModel>
#include <QMimeData>
#include <QDebug>
#include <QHash>
#include <QList>

using namespace google::protobuf;
using CppType = FieldDescriptor::CppType;
using TypeCase = buffers::TreeNode::TypeCase;
using TreeNode = buffers::TreeNode;
using Background = buffers::resources::Background;
using Font = buffers::resources::Font;
using Object = buffers::resources::Object;
using Path = buffers::resources::Path;
using Room = buffers::resources::Room;
using Sound = buffers::resources::Sound;
using Sprite = buffers::resources::Sprite;
using Timeline = buffers::resources::Timeline;

using IconMap = std::unordered_map<TypeCase, QIcon>;

// This is a parent to all internal models
class ProtoModel : public QAbstractItemModel {
  Q_OBJECT
 public:
  explicit ProtoModel(QObject *parent, Message *protobuf);

  // A model is "dirty" if the user has made any changes to it since opening the editor.
  // This is mostly used in "Would you like to save?" dialogs when closing editors.
  void SetDirty(bool dirty);
  bool IsDirty();

  // These are convience functions for getting & setting model used almost everywhere in the codebase
  // because model->setData(model->index(row, col), value, role) is a PITA to type / remember.
  //virtual QVariant Data(int row, int column = 0) const = 0;
  //virtual bool SetData(const QVariant &value, int row, int column = 0) = 0;

  // From here down marks QAbstractItemModel functions required to be implemented
  virtual QModelIndex parent(const QModelIndex &) const override;
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
  virtual bool canSetData(const QModelIndex &index) const {
    // use test role to see if setting the data will be successful
    return const_cast<ProtoModel*>(this)->setData(index,QVariant(),Qt::UserRole);
  }
  virtual QVariant data(const QModelIndex &index, int role) const override;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override = 0;
  virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
  virtual Qt::DropActions supportedDropActions() const override;
  virtual QStringList mimeTypes() const override;
  virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;
  virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                    const QModelIndex &parent) override;

 signals:
  // QAbstractItemModel has a datachanged signal but it doesn't store the old values
  // We use old values in some places to revert invalid changes.
  // All model changes are then hooked into a resourceMap::DataChanged signal that can be accessed anywhere
  // for events where the models arent directly nested and need updates.
  // (ie If you changed a sprite you would want to redraw the object in the room)
  void DataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVariant &oldValue = QVariant(0),
                   const QVector<int> &roles = QVector<int>());
  void ResourceRenamed(TypeCase type, const QString &oldName, const QString &newName);

 protected:
  static IconMap iconMap;
  inline QString treeNodeMime() const { return QStringLiteral("RadialGM/buffers.TreeNode"); }

  bool _dirty;
  Message *_protobuf;
  QStringList _mimes;
  QHash<QPersistentModelIndex,QPersistentModelIndex> parents;

  void setupMimes(const Descriptor* desc, QSet<QString>& uniqueMimes, QSet<const Descriptor*>& visitedDesc);
};

#endif
