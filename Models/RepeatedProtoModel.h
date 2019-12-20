#ifndef REPEATEDPROTOMODEL_H
#define REPEATEDPROTOMODEL_H

#include <QAbstractItemModel>
#include <QSharedPointer>

#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/reflection.h>

#include <set>

using namespace google::protobuf;
using CppType = FieldDescriptor::CppType;

class ProtoModel;
using ProtoModelPtr = ProtoModel*;

class RepeatedProtoModel;
using RepeatedProtoModelPtr = RepeatedProtoModel*;

class RepeatedProtoModel : public QAbstractItemModel {
  Q_OBJECT
 public:
  RepeatedProtoModel(Message *protobuf, const FieldDescriptor *field, ProtoModelPtr parent);
  ProtoModelPtr GetParentModel() const;
  void SetParentModel(ProtoModelPtr parent);
  void AddModel(ProtoModelPtr model);
  ProtoModelPtr GetSubModel(int index);
  QVector<ProtoModelPtr>& GetMutableModelList();
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  bool empty() const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  bool setData(int subModelIndex, int dataField, const QVariant& value);
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
  QVariant data(int row, int column = 0) const;
  QVariant data(const QModelIndex &index, int role) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
  bool moveRows(int source, int count, int destination);
  bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent,
                int destinationChild) override;

  // Takes the elements in range [part, right) and move them to `left` by swapping.
  // Rearranges elements so that all those at or after the partition point are
  // moved to the beginning of the range (`left`).
  void SwapBack(int left, int part, int right);

  class RowRemovalOperation {
    RepeatedProtoModelPtr model;
    MutableRepeatedFieldRef<Message> field;
    std::set<int> rows;

   public:
    RowRemovalOperation(RepeatedProtoModelPtr m):
        model(m),
        field(m->protobuf->GetReflection()
               ->GetMutableRepeatedFieldRef<Message>(m->protobuf, m->field)) {}
    void RemoveRow(int row);
    void RemoveRows(int row, int count);
    ~RowRemovalOperation();
  };

 signals:
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVariant &oldValue = QVariant(0),
                   const QVector<int> &roles = QVector<int>());
  void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);

 protected:
  ProtoModelPtr parentModel;
  Message *protobuf;
  const FieldDescriptor *field;
  QVector<ProtoModelPtr> models;
};

#endif  // REPEATEDPROTOMODEL_H
