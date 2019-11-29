#ifndef REPEATEDPROTOMODEL_H
#define REPEATEDPROTOMODEL_H

#include <QAbstractItemModel>

#include <google/protobuf/message.h>

using namespace google::protobuf;
using CppType = FieldDescriptor::CppType;

class ProtoModel;

class RepeatedProtoModel : public QAbstractItemModel {
  Q_OBJECT
 public:
  RepeatedProtoModel(Message *protobuf, const FieldDescriptor *field, ProtoModel *parent);
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
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

 signals:
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVariant &oldValue = QVariant(0),
                   const QVector<int> &roles = QVector<int>());

 protected:
  Message *protobuf;
  const FieldDescriptor *field;
};

#endif  // REPEATEDPROTOMODEL_H
