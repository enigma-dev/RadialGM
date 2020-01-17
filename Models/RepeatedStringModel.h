#ifndef REPEATEDSTRINGMODEL_H
#define REPEATEDSTRINGMODEL_H

#include "RepeatedModel.h"

#include <string>

class RepeatedStringModel : public RepeatedModel<std::string> {
  Q_OBJECT
 public:
  RepeatedStringModel(ProtoModel *parent, Message *message, const FieldDescriptor *field)
      : RepeatedModel<std::string>(parent, message, field,
                                   message->GetReflection()->GetMutableRepeatedFieldRef<std::string>(message, field)) {}

  virtual void AppendNew() override;
  virtual QVariant Data(int row, int column = 0) const override;
  virtual bool SetData(const QVariant &value, int row, int column = 0) override;

  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
  virtual QVariant data(const QModelIndex &index, int role) const override;
  virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;
  virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                            const QModelIndex &parent) override;
};

#endif  // REPEATEDSTRINGMODEL_H
