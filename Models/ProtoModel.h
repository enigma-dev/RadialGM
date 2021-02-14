#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include "treenode.pb.h"
#include "Utils/FieldPath.h"
#include "Utils/SafeCasts.h"

#include <QAbstractItemModel>
#include <QDebug>
#include <QHash>
#include <QIcon>
#include <QList>
#include <QSize>

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
class PrimitiveModel;

class RepeatedBoolModel;
class RepeatedInt32Model;
class RepeatedInt64Model;
class RepeatedUInt32Model;
class RepeatedUInt64Model;
class RepeatedFloatModel;
class RepeatedDoubleModel;
class RepeatedStringModel;

namespace ProtoModel_private {

RGM_BEGIN_SAFE_CAST(SafeCast, ProtoModel);

RGM_DECLARE_SAFE_CAST(SafeCast, MessageModel);
RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedModel);
RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedMessageModel);
RGM_DECLARE_SAFE_CAST(SafeCast, PrimitiveModel);

RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedBoolModel);
RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedInt32Model);
RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedInt64Model);
RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedUInt32Model);
RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedUInt64Model);
RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedFloatModel);
RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedDoubleModel);
RGM_DECLARE_SAFE_CAST(SafeCast, RepeatedStringModel);

} // namespace ProtoModel_private

// This is a parent to all internal models. The layout of the data varies between the model types.
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
class ProtoModel : public QAbstractItemModel {
  Q_OBJECT
 public:
  explicit ProtoModel(QObject *parent, std::string name, const Descriptor *descriptor);
  explicit ProtoModel(ProtoModel *parent, std::string name, const Descriptor *descriptor);

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

  // Contains rendering information (or transformers to use to obtain such information) for particular fields.
  // Stored in MessageModel mappings and in individual RepeatedModel instances.
  struct FieldDisplayConfig {
    FieldDisplayConfig(bool valid = true) : isValid(valid) {}
    bool isValid;

    /// Function that trades an icon ID field (e.g. its name, number, or name fragment) for its actual QIcon.
    typedef std::function<QIcon(const QVariant &)> IconLookupFn;
    /// When specified, this function accepts this field's value and returns the icon for it.
    IconLookupFn icon_lookup_function;

    /// When icon_lookup_functopm offers no icon, this is the default icon to use for this field.
    QString default_icon_name;

    /// Alternative name for field
    QString name;

    // Allows storing icon parameters at the field level so that generalized editors can be configured per-field.
    // Note that these values are not passed to the icon loader and are not considered by the model itself.
    std::optional<QSize> min_icon_size;
    std::optional<QSize> max_icon_size;
  };

  // Points to FieldDisplayConfigs within a message to use when rendering that message.
  // Stored only in MessageModel/RepeatedMessageModel.
  struct MessageDisplayConfig {
    MessageDisplayConfig(bool valid = true) : isValid(valid) {}
    bool isValid;

    /// When specified, the value of the given field is used as the label for this entire enclosing message.
    FieldPath label_field;
    /// A name to use for this node's icon if the icon field is not applicable or not set.
    FieldPath icon_field;

    /// Function that trades an icon ID field (e.g. its name, number, or name fragment) for its actual QIcon.
    typedef std::function<QIcon(const MessageModel *)> IconLookupFn;
    /// When specified, this function accepts an entire model to compute an icon for it.
    IconLookupFn icon_lookup_function;

    /// When icon_field and icon_lookup_function offer no icon, this is the default icon to use for this message.
    QString default_icon_name;
  };

  // ===================================================================================================================
  // == Display customization ==========================================================================================
  // ===================================================================================================================

  struct DisplayConfig {
    /// Set the icon that will be used to display a given message node when no icon field is set on it.
    template<typename T> void SetDefaultIcon(const QString &icon_name) {
      SetDefaultIcon(T::descriptor()->full_name(), icon_name);
    }
    /// Associates a string field with the label of a given message.
    template<typename T, typename... Fields> void SetMessageLabelField(Fields... field_path) {
      SetMessageLabelField(T::descriptor()->full_name(), FieldPath::Of<T>(field_path...));
    }
    /// Associates a string field with the icon of a given message. The field must contain a path to an image file.
    template<typename T, typename... Fields> void SetMessageIconPathField(Fields... field_path) {
      SetMessageIconPathField(T::descriptor()->full_name(), FieldPath::Of<T>(field_path...));
    }
    /// Associates a string field with the icon name of a given message.
    /// The field must contain the icon name to be looked up in the ArtManager.
    template<typename T, typename... Fields> void SetMessageIconIdField(Fields... field_path) {
      SetMessageIconIdField(T::descriptor()->full_name(), FieldPath::Of<T>(field_path...));
    }
    /// Associates a string field with the icon name of a given message.
    /// The field can contain any identifying information that can be mapped to an icon by the specified function.
    template<typename T, typename... Fields>
    void SetMessageIconIdLookup(FieldDisplayConfig::IconLookupFn icon_lookup_function, Fields... field_path) {
      SetMessageIconIdField(T::descriptor()->full_name(), FieldPath::Of<T>(field_path...), icon_lookup_function);
    }

    // Fetch metadata (or the default instance) by its qualified message name.
    const MessageDisplayConfig &GetMessageDisplay(const std::string &message_qname) const;

    // Ditto for fields
    const FieldDisplayConfig &GetFieldDisplay(const std::string &message_qname) const;

   private:
    // These mirror the above, but are not compile-time safe.
    void SetDefaultIcon(const std::string &message, const QString &icon_name);
    void SetMessageIconPathField(const std::string &message, const FieldPath &field_path);
    void SetMessageLabelField(const std::string &message, const FieldPath &field_path);
    void SetMessageIconIdField(const std::string &message, const FieldPath &field_path,
                               FieldDisplayConfig::IconLookupFn icon_lookup_function);

    QMap<std::string, MessageDisplayConfig> message_display_configs_;
    QMap<std::string, FieldDisplayConfig> field_display_configs_;
  };

  /// Retrieve the custom display name for this field from the display metadata.
  /// If no custom name is set, the name of this model's field in its parent model is returned.
  /// If this is a MessageModel with no custom display name and no parent model, the message name is returned.
  virtual QString GetDisplayName() const;

  /// Retrieve the custom display icon for this field from the display metadata.
  /// If an icon lookup function is specified, it will be called with the appropriate argument.
  /// Otherwise, if an icon name field is set, the icon will be looked up by the value of that field.
  /// If neither of these produces a valid icon, the metadata-specified default icon for this model is returned.
  /// If none of these are specified, a null icon is returned.
  virtual QIcon GetDisplayIcon() const;

  // Retrieve field metadata for a field of the given message type. Returns a sentinel if not specified.
  const MessageDisplayConfig &GetMessageDisplay(const std::string &message_qname) const;

  // Retrieve field metadata for a field of the given message type. Returns a sentinel if not specified.
  const FieldDisplayConfig &GetFieldDisplay(const std::string &message_qname) const;

  // These are convience functions for getting & setting model used almost everywhere in the codebase
  // because model->setData(model->index(row, col), value, role) is a PITA to type / remember.
  QVariant Data(const FieldPath &field_path) const {
    if (const ProtoModel *sub = GetSubModel(field_path)) return sub->Data();
    return QVariant();
  }

  QVariant DataOrDefault(const FieldPath &field_path, const QVariant def = QVariant()) const {
    QVariant ret = Data(field_path);
    if (ret.isValid()) return ret;
    else return def;
  }

  bool SetData(const FieldPath &field_path, const QVariant &value) {
    if (ProtoModel *sub = GetSubModel(field_path)) sub->SetData(value);
    return false;
  }
  ProtoModel *GetSubModel(const FieldPath &field_path) {
    return const_cast<ProtoModel*>(const_cast<const ProtoModel*>(this)->GetSubModel(field_path));
  }

  virtual QVariant Data() const = 0;
  virtual bool SetData(const QVariant &value) = 0;
  virtual const ProtoModel *GetSubModel(const FieldPath &field_path) const = 0;

  QVariant DataAtRow(int row, int col = 0) const { return data(index(row, col, QModelIndex())); }
  bool SetDataAtRow(int row, const QVariant &value) { return SetDataAtRow(row, 0, value); }
  bool SetDataAtRow(int row, int col, const QVariant &value) {
    return setData(index(row, col, QModelIndex()), value);
  }

  const Descriptor *GetDescriptor() const { return descriptor_; }

  // Casting helpers.
  virtual QString DebugName() const = 0;

  virtual MessageModel         *TryCastAsMessageModel()         { return nullptr; }
  virtual RepeatedMessageModel *TryCastAsRepeatedMessageModel() { return nullptr; }
  virtual RepeatedModel        *TryCastAsRepeatedModel()        { return nullptr; }
  virtual PrimitiveModel       *TryCastAsPrimitiveModel()       { return nullptr; }
  virtual RepeatedBoolModel    *TryCastAsRepeatedBoolModel()    { return nullptr; }
  virtual RepeatedInt32Model   *TryCastAsRepeatedInt32Model()   { return nullptr; }
  virtual RepeatedInt64Model   *TryCastAsRepeatedInt64Model()   { return nullptr; }
  virtual RepeatedUInt32Model  *TryCastAsRepeatedUInt32Model()  { return nullptr; }
  virtual RepeatedUInt64Model  *TryCastAsRepeatedUInt64Model()  { return nullptr; }
  virtual RepeatedFloatModel   *TryCastAsRepeatedFloatModel()   { return nullptr; }
  virtual RepeatedDoubleModel  *TryCastAsRepeatedDoubleModel()  { return nullptr; }
  virtual RepeatedStringModel  *TryCastAsRepeatedStringModel()  { return nullptr; }

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
  virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override = 0;
  virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                             int role = Qt::EditRole) override;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override = 0;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

  void SetDisplayConfig(const DisplayConfig &display_config);

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
  const Descriptor *descriptor_;

 private:
   static DisplayConfig display_config_;
};

namespace ProtoModel_private {
RGM_IMPLEMENT_SAFE_CAST(SafeCast, MessageModel);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedModel);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedMessageModel);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, PrimitiveModel);

RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedBoolModel);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedInt32Model);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedInt64Model);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedUInt32Model);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedUInt64Model);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedFloatModel);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedDoubleModel);
RGM_IMPLEMENT_SAFE_CAST(SafeCast, RepeatedStringModel);
} // namespace ProtoModel_private

#endif
