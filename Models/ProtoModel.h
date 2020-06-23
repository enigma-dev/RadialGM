#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include "treenode.pb.h"

#include <QAbstractItemModel>
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

class ProtoModel;

// This is a parent to all internal models
class ProtoModel : public QAbstractItemModel {
  Q_OBJECT
 public:
  explicit ProtoModel(QObject *parent, Message *protobuf);
  explicit ProtoModel(ProtoModel *parent, Message *protobuf);

  // The parent model is the model that own's the current model
  // For resource models like a Room this will be a nullptr
  // For instances it would be a pointer to the room
  // For *a* instance it would be a pointer to a room's instances field's model
  // FIXME: Sanity check this cast
  template <class T>
  T GetParentModel() const {
    return static_cast<T>(_parentModel);
  };

  // If a submodel changed technically any model that owns it has also changed.
  // so we need to notify all parents when anything changes in their descendants.
  void ParentDataChanged();

  // A model is "dirty" if the user has made any changes to it since opening the editor.
  // This is mostly used in "Would you like to save?" dialogs when closing editors.
  void SetDirty(bool dirty);
  bool IsDirty();

  // The layout of the data varies between the model types.
  // For a MessageModel a row is the name of the data field and the column should always be 0.
  //  --------------------------------|
  // | Sprite::kBboxLeftFieldNumber   |
  // |--------------------------------|
  // | Sprite::kBboxRightFieldNumber  |
  // |--------------------------------|
  // | Sprite::kBboxTopFieldNumber    |
  // |--------------------------------|
  // | Sprite::kBboxBottomFieldNumber |
  // |--------------------------------|

  //  For a RepeatedMessage the row is a index in a vector and the column is a data field/
  //  ----------------------------------------------------------------------------------|
  // | Room::Instance 0 | Room::Instance::kXFieldNumber | Room::Instance::kYFieldNumber |
  // |------------------|-------------------------------|-------------------------------|
  // | Room::Instance 1 | Room::Instance::kXFieldNumber | Room::Instance::kYFieldNumber |
  // |------------------|-------------------------------|-------------------------------|
  // | Room::Instance 2 | Room::Instance::kXFieldNumber | Room::Instance::kYFieldNumber |
  // |------------------|-------------------------------|-------------------------------|
  // | Room::Instance 3 | Room::Instance::kXFieldNumber | Room::Instance::kYFieldNumber |
  // |------------------|-------------------------------|-------------------------------|

  // For a RepeatedType like RepeatedString the row is the index and the column should always be 0
  //  ---------------|
  // | "spr_0/1.png" |
  // |---------------|
  // | "spr_0/2.png" |
  // |---------------|
  // | "spr_0/3.png" |

  // These are convience functions for getting & setting model used almost everywhere in the codebase
  // because model->setData(model->index(row, col), value, role) is a PITA to type / remember.
  virtual QVariant Data(int row, int column = 0) const = 0;
  virtual bool SetData(const QVariant &value, int row, int column = 0) = 0;

  // From here down marks QAbstractItemModel functions required to be implemented
  virtual QModelIndex parent(const QModelIndex &) const override;
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override = 0;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override = 0;
  virtual QVariant data(const QModelIndex &index, int role) const override = 0;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override = 0;
  virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override = 0;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

 signals:
  // QAbstractItemModel has a datachanged signal but it doesn't store the old values
  // We use old values in some places to revert invalid changes.
  // All model changes are then hooked into a resourceMap::DataChanged signal that can be accessed anywhere
  // for events where the models arent directly nested and need updates.
  // (ie If you changed a sprite you would want to redraw the object in the room)
  void DataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVariant &oldValue = QVariant(0),
                   const QVector<int> &roles = QVector<int>());

 protected:
  bool _dirty;
  Message *_protobuf;
  ProtoModel *_parentModel;
  const Descriptor *_desc;
  const Reflection *_refl;
};

#endif
