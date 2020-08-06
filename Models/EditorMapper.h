#ifndef EDITORMAPPER_H
#define EDITORMAPPER_H

#include "ImmediateMapper.h"

#include <google/protobuf/message.h>

class EditorModel;
class BaseEditor;

class EditorMapper : public QObject {
 public:
  explicit EditorMapper(EditorModel* model, BaseEditor* parent);

  // mapper
  void addMapping(QWidget *widget, int section, QByteArray propName = "");
  void clearMapping();
  void toFirst();

 protected:
  EditorModel* _model;
  ImmediateDataWidgetMapper *_mapper;
};

#endif  // EDITORMAPPER_H
