#ifndef OBJECTEDITOR_H
#define OBJECTEDITOR_H

#include "BaseEditor.h"

namespace Ui {
class ObjectEditor;
}

class ObjectEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit ObjectEditor(EditorModel* model, QWidget* parent);
  ~ObjectEditor() override;

 private:
  Ui::ObjectEditor* _ui;
};

#endif  // OBJECTEDITOR_H
