#ifndef MODELMAPPER_H
#define MODELMAPPER_H

#include "Models/ImmediateMapper.h"
#include "Models/ProtoModel.h"

#include <google/protobuf/message.h>

class BaseEditor;

class ModelMapper : public QObject {
 public:
  ModelMapper(ProtoModelPtr model, BaseEditor *parent);

  // mapper
  void addMapping(QWidget *widget, int section, QByteArray propName = "");
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
  //QVariant data(int row, int column = 0) const;
  QVariant data(const QModelIndex &index, int role) const;
  //RepeatedProtoModelPtr GetRepeatedSubModel(int fieldNum);
  //RepeatedStringModelPtr GetRepeatedStringSubModel(int fieldNum);
  //ProtoModelPtr GetSubModel(int fieldNum);
  QModelIndex parent(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;

 protected:
  ProtoModelPtr model;
  ImmediateDataWidgetMapper *mapper;
};

#endif  // MODELMAPPER_H
