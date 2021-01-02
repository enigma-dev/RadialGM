#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include "treenode.pb.h"
#include "Utils/FieldPath.h"
#include "Utils/SafeCasts.h"

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
class MessageModel;
class RepeatedModel;
class RepeatedMessageModel;
class RepeatedStringModel;
class RepeatedImageModel;

namespace ProtoModel_private {

RGM_BEGIN_SAFE_CAST(SafeCast, ProtoModel);

RGM_DECLARE_SAFE_CAST(SafeCast, MessageModel);
RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedModel);
RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedMessageModel);
RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedStringModel);
RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedImageModel);

} // namespace ProtoModel_private

// This is a parent to all internal models
class ProtoModel : public QAbstractItemModel {
  Q_OBJECT
 public:
  explicit ProtoModel(QObject *parent, std::string name);
  explicit ProtoModel(ProtoModel *parent, std::string name);

  // The parent model is the model that owns the current model.
  // For the Project model (represented as the resource tree), this will be nullptr.
  // For resource models like a Room, this will be the main Project model.
  // For a room's `instances` list, it will be a pointer to the containing room.
  // For a specific instance, it will be a pointer to a room's `instances` field's model.
  // FIXME: Sanity check this cast
  template <class T> auto *GetParentModel() const { return _parentModel ? _parentModel->As<T>() : nullptr; };

  // If a submodel changed technically any model that owns it has also changed.
  // so we need to notify all parents when anything changes in their descendants.
  void ParentDataChanged();

  // A model is "dirty" if the user has made any changes to it since opening the editor.
  // This is mostly used in "Would you like to save?" dialogs when closing editors.
  void SetDirty(bool dirty);
  bool IsDirty();

  // The layout of the data varies between the model types.
  // For a MessageModel a row is the name of the data field and the column should always be 0.
  // ╭────────────────────────────────╮
  // │ Sprite::kBboxLeftFieldNumber   │
  // ├────────────────────────────────┤
  // │ Sprite::kBboxRightFieldNumber  │
  // ├────────────────────────────────┤
  // │ Sprite::kBboxTopFieldNumber    │
  // ├────────────────────────────────┤
  // │ Sprite::kBboxBottomFieldNumber │
  // ╰────────────────────────────────╯

  //  For a RepeatedMessage the row is a index in a vector and the column is a data field/
  // ╭──────────────────┬───────────────────────────────┬───────────────────────────────╮
  // │ Room::Instance 0 │ Room::Instance::kXFieldNumber │ Room::Instance::kYFieldNumber │
  // ├──────────────────┼───────────────────────────────┼───────────────────────────────┤
  // │ Room::Instance 1 │ Room::Instance::kXFieldNumber │ Room::Instance::kYFieldNumber │
  // ├──────────────────┼───────────────────────────────┼───────────────────────────────┤
  // │ Room::Instance 2 │ Room::Instance::kXFieldNumber │ Room::Instance::kYFieldNumber │
  // ├──────────────────┼───────────────────────────────┼───────────────────────────────┤
  // │ Room::Instance 3 │ Room::Instance::kXFieldNumber │ Room::Instance::kYFieldNumber │
  // ╰──────────────────┴───────────────────────────────┴───────────────────────────────╯

  // For a RepeatedType like RepeatedString the row is the index and the column should always be 0
  // ╭───────────────╮
  // │ "spr_0/1.png" │
  // ├───────────────┤
  // │ "spr_0/2.png" │
  // ├───────────────┤
  // │ "spr_0/3.png" │
  // ╰───────────────╯

  // These are convience functions for getting & setting model used almost everywhere in the codebase
  // because model->setData(model->index(row, col), value, role) is a PITA to type / remember.
  virtual QVariant Data(int row, int column = 0) const = 0;
  virtual QVariant Data(const FieldPath &field_path) const = 0;
  virtual bool SetData(const QVariant &value, int row, int column = 0) = 0;
  virtual bool SetData(const FieldPath &field_path, const QVariant &value) = 0;

  // Casting helpers.
  virtual QString DebugName() const = 0;

  virtual MessageModel *TryCastAsMessageModel() { return nullptr; }
  virtual RepeatedMessageModel *TryCastAsRepeatedMessageModel() { return nullptr; }
  virtual RepeatedModel *TryCastAsRepeatedModel() { return nullptr; }
  virtual RepeatedStringModel *TryCastAsRepeatedStringModel() { return nullptr; }
  virtual RepeatedImageModel *TryCastAsRepeatedImageModel() { return nullptr; }

  /// Returns true if casting from ProtoModel* to T could ever work.
  /// This is essentially std::is_base_of, but with safe casting in mind.
  template<typename T>
  using EnableIfCastable = typename std::enable_if<ProtoModel_private::SafeCast<T>::kCastSupported, bool>::type;
  /// Helper to cast this as some arbitrary choice of the above.
  template<typename T, EnableIfCastable<T> = true> auto* As() {
    auto *res = ProtoModel_private::SafeCast<T>::Cast(this);
    if (!res) qDebug() << "Invalid cast from " << DebugName() << " to " << ProtoModel_private::SafeCast<T>::kName;
    return res;
  }
  template<typename T, EnableIfCastable<T> = true>
  auto* TryCast() { return ProtoModel_private::SafeCast<T>::Cast(this); }

  // Returns the FieldDescriptor describing the given row.
  // The trick is that all nested proto data has some field attached, whether it's primitive, repeated, whatever.
  // Thas model itself may represent a repeated primitive, which has no *Message* Descriptor, but has one
  // FieldDescriptor which applies to all rows in the model.
  virtual const FieldDescriptor *GetRowDescriptor(int row) const = 0;

  // From here down marks QAbstractItemModel functions required to be implemented
  virtual QModelIndex parent(const QModelIndex &) const override;
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override = 0;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override = 0;
  virtual QVariant data(const QModelIndex &index, int role) const override = 0;
  virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                             int role = Qt::EditRole) override;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
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
  ProtoModel *_parentModel;
  const std::string _debug_path;
  QHash<int,QHash<Qt::ItemDataRole,QVariant>> _horizontalHeaderData;
  QHash<int,QHash<Qt::ItemDataRole,QVariant>> _verticalHeaderData;
};

namespace ProtoModel_private {
RGM_IMPLEMENT_SAFE_CAST(SafeCast, MessageModel);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedModel);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedMessageModel);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedImageModel);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedStringModel);
} // namespace ProtoModel_private

#endif
