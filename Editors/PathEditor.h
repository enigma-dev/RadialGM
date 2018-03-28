#ifndef PATHEDITOR_H
#define PATHEDITOR_H

#include "BaseEditor.h"

namespace Ui {
class PathEditor;
}

class PathEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit PathEditor(ProtoModel* model, QWidget* parent);
  ~PathEditor();

 private:
  Ui::PathEditor* ui;
};

#endif  // PATHEDITOR_H
