#ifndef REPEATEDSTRINGMODEL_H
#define REPEATEDSTRINGMODEL_H

#include "RepeatedModel.h"

#include <string>

class RepeatedStringModel : public RepeatedModel<std::string> {
 public:
  RepeatedStringModel(ProtoModelPtr parent, MutableRepeatedFieldRef<std::string> field)
      : RepeatedModel<std::string>(parent, field) {}

  virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;
  virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                            const QModelIndex &parent) override;
};

#endif  // REPEATEDSTRINGMODEL_H
