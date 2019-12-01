#ifndef REPEATEDPROTOMODEL_H
#define REPEATEDPROTOMODEL_H

#include <QAbstractItemModel>

#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/reflection.h>

#include <optional>
#include <QDebug>

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

  class RowRemovalOperation {
      int left = 0, right = 0;
      MutableRepeatedFieldRef<Message> field;
      RepeatedProtoModel *model;
   public:
    RowRemovalOperation(RepeatedProtoModel *m):
        field(m->protobuf->GetReflection()
               ->GetMutableRepeatedFieldRef<Message>(m->protobuf, m->field)),
        model(m) {}
    void RemoveRow(int row) { return RemoveRows(row, 1); }
    void RemoveRows(int row, int count) {
      model->beginRemoveRows(QModelIndex(), row, row + count - 1);
      if (left < right) {
          while (right < row) {
          field.SwapElements(left++, right++);
        }
      } else {
        left = row;
      }
      right = row + count;
      model->endRemoveRows();
    }
    ~RowRemovalOperation() {
      qDebug() << left << "," << right << "," << field.size();
      if (left < right) {
        qDebug() << "swap final " << field.size() - right << " rows backward";
        while (right < field.size()) field.SwapElements(left++, right++);
        qDebug() << "remove final " << field.size() - left << " rows";
        while (left < field.size()) field.RemoveLast();
      }
    }
  };

 signals:
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVariant &oldValue = QVariant(0),
                   const QVector<int> &roles = QVector<int>());
  void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);

 protected:
  Message *protobuf;
  const FieldDescriptor *field;

};

#endif  // REPEATEDPROTOMODEL_H
