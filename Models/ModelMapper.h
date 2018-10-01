#ifndef MODELMAPPER_H
#define MODELMAPPER_H

#include "Models/ProtoModel.h"
#include "Models/ImmediateMapper.h"

#include <google/protobuf/message.h>

class BaseEditor;

class ModelMapper {
public:
  ModelMapper(ProtoModel *model, BaseEditor* parent);

  // mapper
  void addMapping(QWidget * widget, int section);
  void clearMapping();
  void toFirst();

  // model
  void RestoreBuffer();
  void ReplaceBuffer(google::protobuf::Message *buffer);
  void SetDirty(bool dirty);
  bool IsDirty();
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);
  QVariant data(int index) const;
  QVariant data(const QModelIndex &index, int role) const;
  ProtoModel* GetSubModel(int fieldNum);
  QString GetString(int fieldNum, int index);
  QModelIndex parent(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;

protected:
  ProtoModel *model;
  ImmediateDataWidgetMapper *mapper;
};

#endif // MODELMAPPER_H
