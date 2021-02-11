#ifndef REPEATEDMESSAGEMODEL_H
#define REPEATEDMESSAGEMODEL_H

#include "RepeatedModel.h"

class RepeatedMessageModel : public BasicRepeatedModel<Message> {
  Q_OBJECT
 public:
  RepeatedMessageModel(ProtoModel *parent, Message *message, const FieldDescriptor *field);

  // RepeatedMessage models hold multiple MessageModels
  // You can access a submodel by its position within the data structure.
  // (e.g. instancesModel->GetSubmodel(3))
  // XXX: Why would anyone try to access these as anything other than MessageModel...?
  template<typename T> auto *GetSubModel(int index) const {
    return (index < 0 || index > _subModels.size()) ? nullptr: ((ProtoModel*) _subModels[index])->As<T>();
  }

  ProtoModel *GetSubModel(int index) const override {
    return (index < 0 || index > _subModels.size()) ? nullptr : (ProtoModel*) _subModels[index];
  }

  // Translates an underlying Protocol Buffer tag (field number) to the column number from this model.
  int FieldToColumn(int field_number) const {
    const FieldDescriptor *field = field_->message_type()->FindFieldByNumber(field_number);
    if (field) return field->index();
    qDebug() << "Looking up bad field number " << field_number
             << " in RepeatedMessageModel " << GetDescriptor()->full_name().c_str();
    return -1;
  }

  void SwapWithoutSignal(int /*left*/, int /*right*/) override;
  void AppendNewWithoutSignal() override;
  void RemoveLastNRowsWithoutSignal(int /*newSize*/) override;
  void ClearWithoutSignal() override;

  using ProtoModel::Data;
  using ProtoModel::SetData;
  QVariant Data() const override;
  bool SetData(const QVariant &value) override;
  const ProtoModel *GetSubModel(const FieldPath &field_path) const override;

  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
  QVariant data(const QModelIndex &index, int role) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;

  const std::string &MessageName() const;

  QString DebugName() const override {
    return QString::fromStdString("RepeatedMessageModel<" + field_->full_name() + ">");
  }
  RepeatedMessageModel *TryCastAsRepeatedMessageModel() override { return this; }

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
};

#endif
