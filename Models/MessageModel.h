#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include "ProtoModel.h"

// Model representing a protobuf message
class MessageModel : public ProtoModel {
 public:
  MessageModel(ProtoModelPtr parent);
  // On either intialization or restore of a model all
  // refrences to to the submodels it owns recursively must be updated
  void RebuildSubModels();

  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override = 0;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override = 0;
  virtual QVariant data(const QModelIndex &index, int role) const override = 0;
  virtual QModelIndex parent(const QModelIndex &index) const override = 0;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override = 0;
  virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override = 0;
};

#endif
