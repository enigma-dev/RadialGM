#ifndef REPEATEDMODEL_H
#define REPEATEDMODEL_H

#include "ProtoModel.h"
#include "Utils/ProtoManip.h"
#include "Components/Logger.h"

#include <google/protobuf/message.h>
#include <google/protobuf/reflection.h>
#include <google/protobuf/repeated_field.h>

// Model representing a repeated field. Do not instantiate an object of this class directly
class RepeatedModel : public ProtoModel {
 public:
  RepeatedModel(ProtoModel *parent, Message *message, const FieldDescriptor *field)
      : ProtoModel(parent, message->GetDescriptor()->name(), message->GetDescriptor(), field->index()),
        _protobuf(message), field_(field) {}

  bool Empty() { return rowCount() == 0; }

  using ProtoModel::Data;
  using ProtoModel::SetData;
  QVariant Data() const override;
  bool SetData(const QVariant &value) override;
  const ProtoModel *GetSubModel(const FieldPath &field_path) const override;

  // Does the fastest possible conversion from field to QString. Returns empty for message fields.
  virtual QString FastGetQString(int row) const  { (void) row; return {}; }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;

  // Return the submodel serving as the view of the node at the given index.
  virtual ProtoModel *GetSubModel(int index) const = 0;

  QString DebugName() const override {
    return QString::fromStdString("RepeatedModel<" + field_->full_name() + ">");
  }
  RepeatedModel *TryCastAsRepeatedModel() override { return this; }

  /// Represents data in this model as a string. Caution: can be enormous.
  QString DataDebugString() const;

  const FieldDescriptor *GetRowDescriptor(int row) const override {
    Q_UNUSED(row);  // All rows of a repeated field have the same descriptor.
    return field_;
  }

  void Clear() {
    beginResetModel();
    ClearWithoutSignal();
    endResetModel();
    ParentDataChanged();
  }

  const google::protobuf::FieldDescriptor *GetFieldDescriptor() const { return field_; }

  // ===================================================================================================================
  // == Virtual data mutators. =========================================================================================
  // ===================================================================================================================
  // These allow us to implement basic operations in this class without knowing the type of our repeated field.

  // Adds an empty element to the end of the list. Does not emit data change signals.
  virtual void AppendNewWithoutSignal() = 0;
  // Swaps two elements in the underlying model efficiently. Does not emit data change signals.
  virtual void SwapWithoutSignal(int left, int right) = 0;
  // Removes the last N elements from the underlying model efficiently. Does not emit data change signals.
  virtual void RemoveLastNRowsWithoutSignal(int n) = 0;
  // Clears all data in the underlying model. Does not emit data change signals.
  virtual void ClearWithoutSignal() = 0;

  // Directly update the given row with the given value. The caller will have performed bounds checking.
  // Type checking (and conversion, where possible) is on the implementer.
  virtual bool SetDirect(int row, const QVariant &value) = 0;
  // Directly fetch the given row as a Variant. The caller will have performed bounds checking.
  virtual QVariant GetDirect(int row) const = 0;

  // Takes the elements in range [part, right) and move them to `left` by swapping.
  // Rearranges elements so that all those at or after the partition point are
  // moved to the beginning of the range (`left`). Does not emit data change signals.
  void SwapBackWithoutSignal(int left, int part, int right) {
    if (left >= part || part >= right) return;
    int npart = (part - left) % (right - part);
    while (part > left) {
      SwapWithoutSignal(--part, --right);
    }
    SwapBackWithoutSignal(left, left + npart, right);
  }

  // ===================================================================================================================
  // == Moves / deletion / addition - Qt implementations for repeated fields. ==========================================
  // ===================================================================================================================

  bool moveRows(const QModelIndex &sourceParent, int source, int count,
                const QModelIndex &destinationParent, int destination) override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  // Convenience function for internal moves
  bool moveRows(int source, int count, int destination);
  bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
  bool removeRows(int position, int count, const QModelIndex& parent = QModelIndex()) override;
  QMimeData *mimeData(const QModelIndexList &indexes) const override;
  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                    const QModelIndex &parent) override;

  // Mimedata stuff required for Drag & Drop and clipboard functions
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  Qt::DropActions supportedDropActions() const override;
  QStringList mimeTypes() const override;

  class RowRemovalOperation {
   public:
    void RemoveRow(int row) { rows_.insert(row); }
    void RemoveRows(int row, int count) {
      for (int i = row; i < row + count; ++i) rows_.insert(i);
    }
    void RemoveRows(const QModelIndexList& indexes) {
      foreach (auto index, indexes)
        RemoveRow(index.row());
    }

    /// Only allowable ctor
    RowRemovalOperation(RepeatedModel *model) : model_(*model) {}
    // Forbid copy, allow move.
    RowRemovalOperation(const RowRemovalOperation&) = delete;
    RowRemovalOperation(RowRemovalOperation&&) = default;
    /// This method completes the row removal.
    ~RowRemovalOperation();

   private:
    std::set<int> rows_;
    RepeatedModel &model_;
  };

 protected:
  Message *_protobuf;
  const FieldDescriptor *field_;
};

// Model representing a repeated field. Do not instantiate an object of this class directly
template<typename T>
class BasicRepeatedModel : public RepeatedModel {
 public:
  BasicRepeatedModel(ProtoModel *parent, Message *message, const FieldDescriptor *field,
                     MutableRepeatedFieldRef<T> field_ref)
      : RepeatedModel(parent, message, field), field_ref_(field_ref) {}

  // Used to apply changes to any underlying data structure if needed
  // virtual void Swap(int /*left*/, int /*right*/) = 0;
  // virtual void Resize(int /*newSize*/) = 0;
  void ClearWithoutSignal() override { field_ref_.Clear(); }

  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override {
    if (parent.isValid()) return 0;
    return field_ref_.size();
  }

  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override {
    if (parent.isValid()) return 0;
    return 1;
  }

  virtual bool SetDirect(int row, const QVariant &value) override { return SetField(field_ref_, row, value); }
  virtual QVariant GetDirect(int row) const override { return GetField(field_ref_, row); }

  void SwapWithoutSignal(int left, int right) override {
    field_ref_.SwapElements(left, right);
  }

  void RemoveLastNRowsWithoutSignal(int n) override {
    R_EXPECT_V(n <= field_ref_.size())
        << "Trying to remove " << n << " rows from a " << field_ref_.size() << "-row field.";
    for (int j = 0; j < n; ++j) field_ref_.RemoveLast();
  }

 protected:
  MutableRepeatedFieldRef<T> GetfieldRef() const { return field_ref_; }
  MutableRepeatedFieldRef<T> field_ref_;
};

#endif
