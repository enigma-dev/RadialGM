#ifndef PATHEDITOR_H
#define PATHEDITOR_H

#include "BaseEdtior.h"

namespace Ui {
class PathEditor;
}

class PathEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit PathEditor(QWidget *parent, ProtoModel *model);
  ~PathEditor();

 private:
  Ui::PathEditor *ui;
};

#endif  // PATHEDITOR_H
