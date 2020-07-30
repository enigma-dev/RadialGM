#ifndef MODELMAPPER_H
#define MODELMAPPER_H

#include "ProtoModel.h"
#include "ImmediateMapper.h"

#include <google/protobuf/message.h>

class BaseEditor;

class ModelMapper : public QObject {
 public:
  explicit ModelMapper(ProtoModel* model, BaseEditor* parent);

  // mapper
  void addMapping(QWidget *widget, int section, QByteArray propName = "");
  void clearMapping();
  void toFirst();

  // model
  ProtoModel* GetModel();
  void ReplaceBuffer(google::protobuf::Message *buffer);
  bool RestoreBackup();
  void SetDirty(bool dirty);
  bool IsDirty();

 protected:
  ProtoModel* _model;
  ImmediateDataWidgetMapper *_mapper;
};

#endif  // MODELMAPPER_H
