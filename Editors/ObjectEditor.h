#ifndef OBJECTEDITOR_H
#define OBJECTEDITOR_H

#include "BaseEditor.h"

namespace Ui {
class ObjectEditor;
}

class ObjectEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit ObjectEditor(ProtoModelPtr model, QWidget* parent);
  ~ObjectEditor();

 private:
  Ui::ObjectEditor* ui;
};

#endif  // OBJECTEDITOR_H
