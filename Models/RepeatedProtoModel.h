#ifndef REPEATEDPROTOMODEL_H
#define REPEATEDPROTOMODEL_H

#include <QAbstractItemModel>
#include <QDebug>
#include <QSharedPointer>

#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/reflection.h>

#include <optional>
#include <set>

using namespace google::protobuf;
using CppType = FieldDescriptor::CppType;

class ProtoModel;
using ProtoModelPtr = QSharedPointer<ProtoModel>;
using ProtoModelBarePtr = ProtoModel*;

class RepeatedProtoModel;
using RepeatedProtoModelPtr = QSharedPointer<RepeatedProtoModel>;

class RepeatedProtoModel : public QAbstractItemModel {
  Q_OBJECT
 public:
  RepeatedProtoModel(Message *protobuf, const FieldDescriptor *field, ProtoModelBarePtr parent);
  ProtoModelBarePtr GetParentModel() const;
  void AddModel(ProtoModelPtr model);
  ProtoModelPtr GetSubModel(int index);
  QVector<ProtoModelPtr>& GetMutableModelList();
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  bool empty() const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  //bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QVariant data(int row, int column = 0) const;
  QVariant data(const QModelIndex &index, int role) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;

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
  ProtoModelBarePtr parentModel;
  Message *protobuf;
  const FieldDescriptor *field;
  QVector<ProtoModelPtr> models;
};

#endif  // REPEATEDPROTOMODEL_H
