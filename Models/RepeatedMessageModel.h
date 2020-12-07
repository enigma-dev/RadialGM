#ifndef REPEATEDMESSAGEMODEL_H
#define REPEATEDMESSAGEMODEL_H

#include "RepeatedModel.h"

class RepeatedMessageModel : public RepeatedModel<Message> {
  Q_OBJECT
 public:
  RepeatedMessageModel(ProtoModel *parent, Message *message, const FieldDescriptor *field);

  // RepeatedMessage models hold multiple MessageModels
  // You can access a submodel by it's position within the data structure.
  // (ie instancesModel->GetSubmodel(3))
  // FIXME: Sanity check this cast
  template<typename T, typename RType = typename std::remove_pointer<T>::type, EnabeIfCastable<RType> = true>
  RType* GetSubModel(int index) const {
    if (index < 0 || index > _subModels.size()) return nullptr;
    return ((ProtoModel*) _subModels[index])->As<RType>();
  }

  void Swap(int /*left*/, int /*right*/) override;
  void AppendNew() override;
  void Resize(int /*newSize*/) override;
  void Clear() override;

  QVariant Data(int row, int column) const override;
  bool SetData(const QVariant &value, int row, int column) override;
  bool SetData(const FieldPath &field_path, const QVariant &value) override;

  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
  QVariant data(const QModelIndex &index, int role) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;

  const std::string &MessageName() const;
  const google::protobuf::Descriptor *GetDescriptor() const { return _descriptor; }

  /// Inserts the given message as a child at the given row.
  QModelIndex insert(const Message &message, int row);
  /// Duplicates the child at the given index. Returns the index of the new (duplicate) node.
  QModelIndex duplicate(const QModelIndex &message);
  // TODO: implement dropping a message
  //virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;
  //virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
  //const QModelIndex &parent) override;

 protected:
  QVector<MessageModel *> _subModels;
  const google::protobuf::Descriptor *const _descriptor;
};

#endif
