#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include "ProtoModel.h"

#include <google/protobuf/message.h>
#include <google/protobuf/reflection.h>
#include <google/protobuf/repeated_field.h>

// Model representing a repeated field. Do not instantiate an object of this class directly
template <class T>
class RepeatedModel : public ProtoModel {
 public:
  RepeatedModel(ProtoModelPtr parent, MutableRepeatedFieldRef<T> field) : ProtoModel(parent), _field(field) {}

  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override { return 0; }

  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override { return 0; }

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

  // Convience function for internal moves
  bool moveRows(int source, int count, int destination) {
    return moveRows(QModelIndex(), source, count, QModelIndex(), destination);
  }

  virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override {
    if (row > rowCount()) return false;

    beginInsertRows(parent, row, row + count - 1);

    int p = rowCount();

    for (int r = 0; r < count; ++r) {
      _field.Add(T());
      if (std::is_same<T, Message>::value) {
        _subModels.append(_field.Get(rowCount() - 1));
      }
    }

    SwapBack(row, p, rowCount());
    ParentDataChanged();

    return true;
  };

  virtual bool removeRows(int position, int count, const QModelIndex & /*parent*/) override {
    RowRemovalOperation remover(this, _field, _subModels);
    remover.RemoveRows(position, count);
    return true;
  }

  // Mimedata stuff required for Drag & Drop and clipboard functions
  virtual Qt::DropActions supportedDropActions() { return Qt::MoveAction | Qt::CopyAction; }

  virtual QMimeData *mimeData(const QModelIndexList & /*indexes*/) const override {}
  virtual bool dropMimeData(const QMimeData * /*data*/, Qt::DropAction /*action*/, int /*row*/, int /*column*/,
                            const QModelIndex & /*parent*/) override {}

  virtual QStringList mimeTypes() const override {
    return QStringList("RadialGM/"); /*QString::fromStdString(_field.name());*/
  }

  // Takes the elements in range [part, right) and move them to `left` by swapping.
  // Rearranges elements so that all those at or after the partition point are
  // moved to the beginning of the range (`left`).
  void SwapBack(int left, int part, int right) {
    if (left >= part || part >= right) return;
    int npart = (part - left) % (right - part);
    while (part > left) {
      if (_subModels.empty()) std::swap(_subModels[left], _subModels[right]);
      _field.SwapElements(part, right);
    }
    SwapBack(left, left + npart, right);
  }

  class RowRemovalOperation {
    std::set<int> _rows;
    RepeatedModel<T> _model;
    MutableRepeatedFieldRef<T> _field;
    QVector<ProtoModelPtr> *_subModels;

   public:
    RowRemovalOperation(RepeatedModel<T> model, MutableRepeatedFieldRef<T> field, QVector<ProtoModelPtr> *subModels)
        : _model(model), _field(field), _subModels(subModels) {}
    void RemoveRow(int row) { _rows.insert(row); }
    void RemoveRows(int row, int count) {
      for (int i = row; i < row + count; ++i) _rows.insert(i);
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

      emit _model->beginResetModel();

      // Basic dense range removal. Move "deleted" rows to the end of the array.
      int left = 0, right = 0;
      for (auto range : ranges) {
        while (right < range.first) {
          _field.SwapElements(left, right);
          if (_subModels->empty()) std::swap(_subModels[left], _subModels[right]);
          left++;
          right++;
        }
        right = range.last + 1;
      }
      while (right < _field.size()) {
        _field.SwapElements(left, right);
        if (_subModels->empty()) std::swap(_subModels[left], _subModels[right]);
        left++;
        right++;
      }

      // Send the endRemoveRows operations in the reverse order, removing the
      // correct number of rows incrementally, or else various components in Qt
      // will bitch, piss, moan, wail, whine, and cry. Actually, they will anyway.
      for (Range range : ranges) {
        if (_subModels->empty()) _subModels->resize(_subModels->size() - range.size());
        for (int j = range.first; j <= range.last; ++j) _field.RemoveLast();
      }

      emit _model->endResetModel();

      _model->GetParentModel()->SetDirty(true);
    }
  };

 protected:
  MutableRepeatedFieldRef<T> _field;
};

#endif
