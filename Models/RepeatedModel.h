#ifndef REPEATEDMODEL_H
#define REPEATEDMODEL_H

#include "ProtoModel.h"
#include "PrimitiveModel.h"
#include "Utils/ProtoManip.h"

#include <google/protobuf/message.h>
#include <google/protobuf/reflection.h>
#include <google/protobuf/repeated_field.h>

// Model representing a repeated field. Do not instantiate an object of this class directly
class RepeatedModel : public ProtoModel {
 public:
  RepeatedModel(ProtoModel *parent, Message *message, const FieldDescriptor *field)
      : ProtoModel(parent, message->GetDescriptor()->name(), message->GetDescriptor()), _protobuf(message),
       field_(field) {}

  bool Empty() { return rowCount() == 0; }

  using ProtoModel::Data;
  using ProtoModel::SetData;
  QVariant Data() const override;
  bool SetData(const QVariant &value) override;
  const ProtoModel *GetSubModel(const FieldPath &field_path) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;

  // Return the submodel serving as the view of the node at the given index.
  virtual ProtoModel *GetSubModel(int index) const = 0;

  QString DebugName() const override {
    return QString::fromStdString("RepeatedModel<" + field_->full_name() + ">");
  }
  RepeatedModel *TryCastAsRepeatedModel() override { return this; }

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
  QMimeData *mimeData(const QModelIndexList &indexes) const override;
  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                    const QModelIndex &parent) override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  // Convenience function for internal moves
  bool moveRows(int source, int count, int destination);
  bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
  bool removeRows(int position, int count, const QModelIndex& parent = QModelIndex()) override;

  // Mimedata stuff required for Drag & Drop and clipboard functions
  virtual Qt::DropActions supportedDropActions() const override;
  QStringList mimeTypes() const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  class RowRemovalOperation {
   public:
    RowRemovalOperation(RepeatedModel *model) : model_(*model) {}
    void RemoveRow(int row) { rows_.insert(row); }
    void RemoveRows(int row, int count) {
      for (int i = row; i < row + count; ++i) rows_.insert(i);
    }
    void RemoveRows(const QModelIndexList& indexes) {
      foreach (auto index, indexes)
        RemoveRow(index.row());
    }

    ~RowRemovalOperation() {
      if (rows_.empty()) return;

      // Compute ranges for our deleted rows.
      struct Range {
        int first, last;
        Range() : first(), last() {}
        Range(int f, int l) : first(f), last(l) {}
        int size() { return last - first + 1; }
      };
      std::vector<Range> ranges;
      for (int row : rows_) {
        if (ranges.empty() || row != ranges.back().last + 1) {
          ranges.emplace_back(row, row);
        } else {
          ranges.back().last = row;
        }
      }

      model_.beginResetModel();

      // Basic dense range removal. Move "deleted" rows to the end of the array.
      int left = 0, right = 0;
      for (auto range : ranges) {
        while (right < range.first) {
          model_.SwapWithoutSignal(left, right);
          left++;
          right++;
        }
        right = range.last + 1;
      }
      while (right < model_.rowCount()) {
        model_.SwapWithoutSignal(left, right);
        left++;
        right++;
      }

      // Send the endRemoveRows operations in the reverse order, removing the
      // correct number of rows incrementally, or else various components in Qt
      // will bitch, piss, moan, wail, whine, and cry. Actually, they will anyway.
      for (Range range : ranges) {
        model_.RemoveLastNRowsWithoutSignal(range.size());
      }

      model_.endResetModel();

      model_.ParentDataChanged();
    }

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
    for (int j = 0; j <= n; ++j) field_ref_.RemoveLast();
  }

 protected:
  MutableRepeatedFieldRef<T> GetfieldRef() const { return field_ref_; }
  MutableRepeatedFieldRef<T> field_ref_;
};

template <typename T>
class RepeatedPrimitiveModel : public BasicRepeatedModel<T> {
 public:
  RepeatedPrimitiveModel(ProtoModel *parent, Message *message, const FieldDescriptor *field) : BasicRepeatedModel<T>(
      parent, message, field, message->GetReflection()->GetMutableRepeatedFieldRef<T>(message, field)) {
    RebuildSubModels();
  }

  T PrimitiveData(int row) const {
    return BasicRepeatedModel<T>::field_ref_.Get(row);
  }

  // Need to implement this in all RepeatedModels
  void AppendNewWithoutSignal() final {
    BasicRepeatedModel<T>::field_ref_.Add({});
  }

  void RebuildSubModels() {
    submodels_.clear();
    submodels_.reserve(BasicRepeatedModel<T>::field_ref_.size());
    for (int i = 0; i < BasicRepeatedModel<T>::field_ref_.size(); ++i) {
      submodels_.push_back(new PrimitiveModel(this, i));
    }
  }

  ProtoModel *GetSubModel(int index) const final {
    return submodels_[index];
  }

 private:
  QVector<PrimitiveModel*> submodels_;
};

#define RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(ModelName, model_type)                     \
    class ModelName : public RepeatedPrimitiveModel<model_type> {                       \
     public:                                                                            \
      ModelName(ProtoModel *parent, Message *message, const FieldDescriptor *field)     \
          : RepeatedPrimitiveModel<model_type>(parent, message, field) {}               \
                                                                                        \
      QString DebugName() const override {                                              \
        return QString::fromStdString(#ModelName "<" + field_->full_name() + ">");      \
      }                                                                                 \
      ModelName *TryCastAs ## ModelName() override { return this; }                     \
    }

RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedStringModel, std::string);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedBoolModel,   bool);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedInt32Model,  google::protobuf::int32);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedInt64Model,  google::protobuf::int64);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedUInt32Model, google::protobuf::uint32);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedUInt64Model, google::protobuf::uint64);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedFloatModel,  float);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedDoubleModel, double);

#undef RGM_DECLARE_REPEATED_PRIMITIVE_MODEL

#endif
