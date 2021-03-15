#ifndef PRIMITIVEMODEL_H
#define PRIMITIVEMODEL_H

#include "ProtoModel.h"
#include "Utils/ProtoManip.h"

#include <google/protobuf/message.h>
#include <google/protobuf/reflection.h>
#include <google/protobuf/repeated_field.h>

// Model representing a repeated field. Do not instantiate an object of this class directly
class PrimitiveModel : public ProtoModel {
 public:
  // Construct a repeated field index.
  PrimitiveModel(ProtoModel *parent, int row_in_parent)
      : ProtoModel(parent, parent->GetDescriptor()->name(), parent->GetDescriptor(), row_in_parent),
        field_or_null_(nullptr) {}
  PrimitiveModel(ProtoModel *parent, const FieldDescriptor *field)
      : ProtoModel(parent, parent->GetDescriptor()->name(), parent->GetDescriptor(), field->index()),
        field_or_null_(field) {}

  bool Empty() { return rowCount() == 0; }

  using ProtoModel::Data;
  using ProtoModel::SetData;
  QVariant Data() const override {
    return GetDirect();
  }
  bool SetData(const QVariant &value) override {
    return SetDirect(value);
  }
  const ProtoModel *GetSubModel(const FieldPath &field_path) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
    if (index.column() || index.row() || role != Qt::DisplayRole) return QVariant();
    return GetDirect();
  }
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override {
    if (index.column() || index.row() || role != Qt::DisplayRole) return false;
    return SetDirect(value);
  }

  QVariant GetDirect() const {
    return _parentModel->DataAtRow(row_in_parent_);
  }
  bool SetDirect(const QVariant &value) {
    return _parentModel->SetDataAtRow(row_in_parent_, value);
  }

  QString DebugName() const override {
    if (field_or_null_) return QString::fromStdString("PrimitiveModel<" + field_or_null_->full_name() + ">");
    return _parentModel->DebugName() + "[" + QString::number(row_in_parent_) + "]";
  }
  PrimitiveModel *TryCastAsPrimitiveModel() override { return this; }

  const FieldDescriptor *GetRowDescriptor(int row) const override;

  QString GetDisplayName() const override;
  QIcon GetDisplayIcon() const override;

  // ===================================================================================================================
  // == Moves / deletion / addition - Qt implementations for primitive fields. =========================================
  // ===================================================================================================================

  int rowCount(const QModelIndex& = QModelIndex()) const override { return 1; }
  int columnCount(const QModelIndex& = QModelIndex()) const override  { return 1; }
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
    Q_UNUSED(parent);
    if (row || column) return QModelIndex();
    return createIndex(0, 0, (void*) this);
  }

 protected:
  const FieldDescriptor *const field_or_null_;
};

#endif
