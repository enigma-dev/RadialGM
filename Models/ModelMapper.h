#ifndef MODELMAPPER_H
#define MODELMAPPER_H

#include "Models/ImmediateMapper.h"
#include "Models/MessageModel.h"

#include <google/protobuf/message.h>

class BaseEditor;

class ModelMapper : public QObject {
 public:
  ModelMapper(MessageModel *_model, BaseEditor *parent);
  ModelMapper(MessageModel *_model);

  // mapper
  void addMapping(QWidget *widget, int section, QByteArray propName = "");
  void clearMapping();
  void toFirst();

  // model
  MessageModel *GetModel();
  void ReplaceBuffer(google::protobuf::Message *buffer);
  bool RestoreBackup();
  void SetDirty(bool dirty);
  bool IsDirty();

 protected:
  MessageModel *_model;
  ImmediateDataWidgetMapper *_mapper;
};

#endif  // MODELMAPPER_H
