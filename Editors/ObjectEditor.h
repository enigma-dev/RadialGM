#ifndef OBJECTEDITOR_H
#define OBJECTEDITOR_H

#include "BaseEdtior.h"

namespace Ui {
class ObjectEditor;
}

class ObjectEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit ObjectEditor(QWidget *parent, ProtoModel *model);
  ~ObjectEditor();

 private:
  Ui::ObjectEditor *ui;
};

#endif  // OBJECTEDITOR_H
