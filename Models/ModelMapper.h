#ifndef MODELMAPPER_H
#define MODELMAPPER_H

#include "Models/ImmediateMapper.h"
#include "Models/MessageModel.h"

#include <google/protobuf/message.h>

class BaseEditor;

class ModelMapper : public QObject {
 public:
  ModelMapper(MessageModel *_model, BaseEditor *parent);

  // mapper
  void addMapping(QWidget *widget, int section, QByteArray propName = "");
  void clearMapping();

  // model
  MessageModel *GetModel();
  void ReplaceBuffer(google::protobuf::Message *buffer);
  bool RestoreBackup();
  void SetDirty(bool dirty);
  bool IsDirty();

 public slots:
  void setModel(MessageModel *model);
  void toFirst();
  void setOrientation(Qt::Orientation aOrientation);
  void setCurrentIndex(int index); // toFirst(), toLast(), etc.
  void revert();

 protected:
  MessageModel *_model;
  ImmediateDataWidgetMapper *_mapper;
};

#endif  // MODELMAPPER_H
