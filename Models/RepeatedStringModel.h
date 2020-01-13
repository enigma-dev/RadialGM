#ifndef REPEATEDSTRINGMODEL_H
#define REPEATEDSTRINGMODEL_H

#include <QAbstractListModel>
#include <QSize>

#include <google/protobuf/message.h>
#include <google/protobuf/reflection.h>
#include <google/protobuf/repeated_field.h>

#include <set>

using namespace google::protobuf;
using CppType = FieldDescriptor::CppType;

class RepeatedStringModel;
using RepeatedStringModelPtr = RepeatedStringModel*;

class ProtoModel;
using ProtoModelPtr = ProtoModel*;

class RepeatedStringModel : public QAbstractListModel {
  Q_OBJECT

 public:
  RepeatedStringModel(MutableRepeatedFieldRef<std::string> protobuf, const FieldDescriptor* field,
                      ProtoModelPtr parent);
  ProtoModelPtr GetParentModel() const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  virtual bool insertRows(int position, int rows, const QModelIndex& parent) override;
  virtual bool removeRows(int position, int rows, const QModelIndex& parent) override;
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  virtual QVariant data(int row) const;
  virtual bool setData(int row, const QVariant& value);
  virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
  virtual Qt::DropActions supportedDropActions() const override;
  virtual QMimeData* mimeData(const QModelIndexList& indexes) const override;
  virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                            const QModelIndex& parent) override;
  virtual QStringList mimeTypes() const override;
  virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
  void clear();

  // Takes the elements in range [part, right) and move them to `left` by swapping.
  // Rearranges elements so that all those at or after the partition point are
  // moved to the beginning of the range (`left`).
  void SwapBack(int left, int part, int right);

  class RowRemovalOperation {
    RepeatedStringModelPtr model;
    MutableRepeatedFieldRef<std::string> field;
    std::set<int> rows;

   public:
    RowRemovalOperation(RepeatedStringModelPtr m) : model(m), field(m->strings) {}
    void RemoveRow(int row);
    void RemoveRows(int row, int count);
    ~RowRemovalOperation();
  };

 signals:
  void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVariant& oldValue = QVariant(0),
                   const QVector<int>& roles = QVector<int>());
  void rowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);

 protected:
  void ParentDataChanged();
  ProtoModelPtr parentModel;
  const FieldDescriptor* field;
  MutableRepeatedFieldRef<std::string> strings;
  QString mimeTypeStr;
};

using RepeatedStringModelPtr = RepeatedStringModel*;

#endif  // REPEATEDSTRINGMODEL_H
