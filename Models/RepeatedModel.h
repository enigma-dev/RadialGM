#ifndef REPEATEDMODEL_H
#define REPEATEDMODEL_H

#include "ProtoModel.h"

#include <google/protobuf/message.h>
#include <google/protobuf/reflection.h>
#include <google/protobuf/repeated_field.h>

// Model representing a repeated field. Do not instantiate an object of this class directly
template <class T>
class RepeatedModel : public ProtoModel {
 public:
  RepeatedModel(ProtoModel *parent, Message *message, const FieldDescriptor *field, MutableRepeatedFieldRef<T> fieldRef)
      : ProtoModel(parent, message), _field(field), _fieldRef(fieldRef) {}

  // Need to implement this in all RepeatedModels
  virtual void AppendNew() = 0;

  // Used to apply changes to any underlying data structure if needed
  virtual void Swap(int /*left*/, int /*right*/) {}
  virtual void Resize(int /*newSize*/) {}
  virtual void Clear() { _fieldRef.Clear(); }

  bool Empty() { return rowCount() == 0; }

  virtual QVariant Data(int row, int column = 0) const = 0;
  virtual bool SetData(const QVariant &value, int row, int column = 0) = 0;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override = 0;
  virtual QVariant data(const QModelIndex &index, int role) const override = 0;

  // Moves / deletion / addition only make sense in repeated fields

  virtual bool moveRows(const QModelIndex &sourceParent, int source, int count, const QModelIndex &destinationParent,
                        int destination) override {
    int left = source;
    int right = source + count;
    if (left < 0 || destination < 0) return false;
    if (right > rowCount() || destination > rowCount()) return false;
    if (destination >= left && destination < right) return false;

    beginMoveRows(sourceParent, source, source + count - 1, destinationParent, destination);

    if (destination < left) {
      SwapBack(destination, left, right);
    } else {  // Verified above that we're dest < left or dest >= right
      SwapBack(left, right, destination);
    }

    endMoveRows();
    ParentDataChanged();

    return true;
  }

  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override {
    if (parent.isValid()) return 0;
    return _fieldRef.size();
  }

  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override {
    if (parent.isValid()) return 0;
    return 1;
  }

  virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
    Q_UNUSED(parent);
    return this->createIndex(row, column);
  }

  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
    auto data = ProtoModel::headerData(section, orientation, role);
    if (data.isValid()) return data;
    if (section <= 0 || role != Qt::DisplayRole || orientation != Qt::Orientation::Horizontal)
      return QVariant(); // << invalid
    return QString::fromStdString(_field->message_type()->field(section - 1)->name());
  }

  // Convenience function for internal moves
  bool moveRows(int source, int count, int destination) {
    return moveRows(QModelIndex(), source, count, QModelIndex(), destination);
  }

  virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override {
    if (row > rowCount()) return false;

    beginInsertRows(parent, row, row + count - 1);

    int p = rowCount();

    AppendNew();

    SwapBack(row, p, rowCount());
    ParentDataChanged();

    endInsertRows();

    return true;
  };

  virtual bool removeRows(int position, int count, const QModelIndex& parent = QModelIndex()) override {
    Q_UNUSED(parent);
    RowRemovalOperation remover(this);
    remover.RemoveRows(position, count);
    return true;
  }

  // Mimedata stuff required for Drag & Drop and clipboard functions
  virtual Qt::DropActions supportedDropActions() const override { return Qt::MoveAction | Qt::CopyAction; }

  virtual QStringList mimeTypes() const override {
    return QStringList("RadialGM/" + QString::fromStdString(_field->DebugString()));
  }

  virtual Qt::ItemFlags flags(const QModelIndex &index) const override {
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (index.isValid())
      return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
      return Qt::ItemIsDropEnabled | defaultFlags;
  }

  // Takes the elements in range [part, right) and move them to `left` by swapping.
  // Rearranges elements so that all those at or after the partition point are
  // moved to the beginning of the range (`left`).
  void SwapBack(int left, int part, int right) {
    if (left >= part || part >= right) return;
    int npart = (part - left) % (right - part);
    while (part > left) {
      Swap(--part, --right);
      _fieldRef.SwapElements(part, right);
    }
    SwapBack(left, left + npart, right);
  }

  class RowRemovalOperation {
    std::set<int> _rows;
    RepeatedModel<T> &_model;
    MutableRepeatedFieldRef<T> _field;

   public:
    RowRemovalOperation(RepeatedModel<T> *model) : _model(*model), _field(model->GetfieldRef()) {}
    void RemoveRow(int row) { _rows.insert(row); }
    void RemoveRows(int row, int count) {
      for (int i = row; i < row + count; ++i) _rows.insert(i);
    }
    void RemoveRows(const QModelIndexList& indexes) {
      foreach (auto index, indexes)
        RemoveRow(index.row());
    }

    ~RowRemovalOperation() {
      if (_rows.empty()) return;

      // Compute ranges for our deleted rows.
      struct Range {
        int first, last;
        Range() : first(), last() {}
        Range(int f, int l) : first(f), last(l) {}
        int size() { return last - first + 1; }
      };
      std::vector<Range> ranges;
      for (int row : _rows) {
        if (ranges.empty() || row != ranges.back().last + 1) {
          ranges.emplace_back(row, row);
        } else {
          ranges.back().last = row;
        }
      }

      // Basic dense range removal. Move "deleted" rows to the end of the array.
      int left = 0, right = 0;
      for (auto range : ranges) {
        while (right < range.first) {
          _field.SwapElements(left, right);
          _model.Swap(left, right);
          left++;
          right++;
        }
        right = range.last + 1;
      }
      while (right < _field.size()) {
        _field.SwapElements(left, right);
        _model.Swap(left, right);
        left++;
        right++;
      }

      // Send the endRemoveRows operations in the reverse order, removing the
      // correct number of rows incrementally, or else various components in Qt
      // will bitch, piss, moan, wail, whine, and cry. Actually, they will anyway.
      for (Range range : ranges) {
        emit _model.beginRemoveRows(QModelIndex(), range.first, range.last);
        _model.Resize(_field.size() - range.size());
        for (int j = range.first; j <= range.last; ++j) _field.RemoveLast();
        emit _model.endRemoveRows();
      }

      _model.ParentDataChanged();
    }
  };

 protected:
  MutableRepeatedFieldRef<T> GetfieldRef() const { return _fieldRef; }
  const FieldDescriptor *_field;
  MutableRepeatedFieldRef<T> _fieldRef;
};

#endif
