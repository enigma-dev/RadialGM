#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "BaseEditor.h"
#include "ui_CodeEditor.h"

namespace Ui {
class CodeEditor;
}

class CodeEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit CodeEditor(ProtoModel *model, QWidget *parent);
  ~CodeEditor();

 protected:
  Ui::CodeEditor *ui;
};

#endif  // CODEEDITOR_H
