#ifndef PROTOMODEL_H
#define PROTOMODEL_H

#include "treenode.pb.h"
#include "Components/Logger.h"

#include <QAbstractItemModel>
#include <QMimeData>
#include <QHash>
#include <QList>

using namespace google::protobuf;

using TypeCase = buffers::TreeNode::TypeCase;
using TreeNode = buffers::TreeNode;

using IconMap = std::unordered_map<TypeCase, QIcon>;

template <typename T = void>
T* pointer_byte_offset(T* ptr, size_t n) {
  return (T*)((char*)ptr + n);
}
template <typename T = void>
T* field_pointer(T* msg, size_t fieldIndex) {
  // because QModelIndex equality uses row, column, & parent
  // we distinguish fields from messages by byte offsetting
  // the message pointer by 1 plus the field index
  return pointer_byte_offset<T>(msg, fieldIndex+1);
}
template <typename T = void>
T* field_pointer(const QModelIndex& index) {
  return field_pointer<T>(index.internalPointer(), index.row());
}

// This is the mother of all models.
class ProtoModel : public QAbstractItemModel {
  Q_OBJECT

  bool RepeatedMutateSetup(Message** message, const FieldDescriptor** field,
                           const QModelIndex& parent) const;

 public:
  static IconMap iconMap;

  explicit ProtoModel(QObject *parent, Message *protobuf);

  inline Message* GetMessage(const QModelIndex& index) const {
    auto msg = static_cast<Message*>(_protobuf);
    if (index.isValid()) {
      if (IsMessage(index))
        msg = static_cast<Message*>(index.internalPointer());
      else {
        R_EXPECT(index != index.parent(), msg)
            << "Message index equal to parent: " << index;
        msg = GetMessage(index.parent());
      }
    }
    return msg;
  }
  static inline bool IsMessage(const QModelIndex& index) {
    return !index.isValid() || // << root is always message in this model
        (index.parent().isValid() &&
         index.internalPointer() !=
         field_pointer(index.parent().internalPointer(), index.row()));
  }
  static inline bool IsField(const QModelIndex& index) {
    return !IsMessage(index);
  }

  // These are conveniences for finding model indexes for fields & messages.
  inline QModelIndex indexOfField(Message* message, int fieldIndex) const {
    void *ptr = field_pointer(message, fieldIndex);
    return this->createIndex(fieldIndex,0,ptr);
  }
  inline QModelIndex indexOfField(Message* message, const FieldDescriptor* field) const {
    return indexOfField(message,field->index());
  }
  inline QModelIndex indexOfFieldByNumber(Message* message, int fieldNumber) const {
    auto desc = message->GetDescriptor();
    auto field = desc->FindFieldByNumber(fieldNumber);
    return indexOfField(message,field);
  }
  inline QModelIndex indexOfFieldByName(Message* message, std::string fieldName) const {
    auto desc = message->GetDescriptor();
    auto field = desc->FindFieldByName(fieldName);
    return indexOfField(message,field);
  }

  // These are convience functions for getting & setting model used almost everywhere in the codebase
  // because model->setData(model->index(row, col), value, role) is a PITA to type / remember.
  //virtual QVariant Data(int row, int column = 0) const = 0;
  //virtual bool SetData(const QVariant &value, int row, int column = 0) = 0;

  // From here down marks QAbstractItemModel functions required to be implemented
  virtual QModelIndex parent(const QModelIndex &) const override;
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
  virtual bool canSetData(const QModelIndex &index) const {
    // use test role to see if setting the data will be successful
    return const_cast<ProtoModel*>(this)->setData(index,QVariant(),Qt::UserRole);
  }
  virtual QVariant data(const QModelIndex &index, int role) const override;
  //virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  virtual QModelIndex index(int row, int column,
                            const QModelIndex &parent = QModelIndex()) const override;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

  // this is all the generic drag and drop stuff
  virtual Qt::DropActions supportedDropActions() const override;
  virtual QStringList mimeTypes() const override;
  virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;
  virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                               const QModelIndex &parent) const override;
  virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                    const QModelIndex &parent) override;

  // part of the abstract item model API for rearranging the rows
  // used by the generic drag and drop to move stuff around
  virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
  virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                        const QModelIndex &destinationParent, int destinationChild) override;
  virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

 signals:
  void ResourceRenamed(TypeCase type, const QString &oldName, const QString &newName);

 protected:
  inline QString treeNodeMime() const { return QStringLiteral("RadialGM/buffers.TreeNode"); }

  Message *_protobuf;
  QStringList _mimes;
  QHash<QPersistentModelIndex,QPersistentModelIndex> parents;

  void setupMimes(const Descriptor* desc, QSet<QString>& uniqueMimes, QSet<const Descriptor*>& visitedDesc);
};

#endif // PROTOMODEL_H
