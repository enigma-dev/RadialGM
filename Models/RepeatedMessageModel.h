#ifndef REPEATEDMESSAGEMODEL_H
#define REPEATEDMESSAGEMODEL_H

#include "RepeatedModel.h"

class RepeatedMessageModel : public RepeatedModel<Message> {
 public:
  RepeatedMessageModel(ProtoModelPtr parent, MutableRepeatedFieldRef<Message> field)
      : RepeatedModel<Message>(parent, field) {}

  // TODO: implement dropping a message
  //virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;
  //virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
  //const QModelIndex &parent) override;
};

#endif
