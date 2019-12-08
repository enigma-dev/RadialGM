#ifndef MODELMAPPER_H
#define MODELMAPPER_H

#include "Models/ProtoModel.h"
#include "Models/ImmediateMapper.h"

#include <google/protobuf/message.h>

class BaseEditor;

class ModelMapper {
public:
  ModelMapper(ProtoModelPtr model, BaseEditor* parent);

  // mapper
  void addMapping(QWidget* widget, int section);
  void clearMapping();
  void toFirst();

  // model
  ProtoModelPtr GetModel();
  void ReplaceBuffer(google::protobuf::Message *buffer);
  bool RestoreBackup();
  void SetDirty(bool dirty);
  bool IsDirty();
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);
  QVariant data(int row, int column=0) const;
  QVariant data(const QModelIndex &index, int role) const;
  RepeatedProtoModelPtr GetRepeatedSubModel(int fieldNum);
  ProtoModelPtr GetSubModel(int fieldNum);
  QString GetString(int fieldNum, int index);
  QModelIndex parent(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;

protected:
  BaseEditor* parentWidget;
  ProtoModelPtr model;
  ImmediateDataWidgetMapper* mapper;
};

#endif // MODELMAPPER_H
