#ifndef OBJECTEDITOR_H
#define OBJECTEDITOR_H

#include "BaseEditor.h"

namespace Ui {
class ObjectEditor;
}

class ObjectEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit ObjectEditor(MessageModel* model, QWidget* parent);
  ~ObjectEditor() override;

 private slots:
  void on_actionAddEvent_triggered();

 private:
  Ui::ObjectEditor* _ui;
};

#endif  // OBJECTEDITOR_H
