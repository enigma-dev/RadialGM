#ifndef EDITORMAPPER_H
#define EDITORMAPPER_H

#include <QObject>

#include <google/protobuf/message.h>

class EditorModel;
class BaseEditor;

class EditorMapper : public QObject {
  Q_OBJECT

 public:
  explicit EditorMapper(EditorModel* model, BaseEditor* parent);

 protected:
  EditorModel* _model;
};

#endif  // EDITORMAPPER_H
