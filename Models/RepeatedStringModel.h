#ifndef REPEATEDSTRINGMODEL_H
#define REPEATEDSTRINGMODEL_H

#include "RepeatedModel.h"

#include <string>

// TODO: rename this to RepeatedScalarModel or RepeatedPrimitiveModel; it only uses QVariant for its data.
class RepeatedStringModel : public RepeatedModel<std::string> {
  Q_OBJECT
 public:
  RepeatedStringModel(ProtoModel *parent, Message *message, const FieldDescriptor *field)
      : RepeatedModel<std::string>(parent, message, field,
                                   message->GetReflection()->GetMutableRepeatedFieldRef<std::string>(message, field)) {}

  void AppendNew() override;
  QVariant Data(int row, int column = 0) const override;
  bool SetData(const QVariant &value, int row, int column = 0) override;
  bool SetData(const FieldPath &field_path, const QVariant &value) override;

  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
  QVariant data(const QModelIndex &index, int role) const override;
  QMimeData *mimeData(const QModelIndexList &indexes) const override;
  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                    const QModelIndex &parent) override;

  RepeatedStringModel *AsRepeatedStringModel() override { return this; }
};

#endif  // REPEATEDSTRINGMODEL_H
