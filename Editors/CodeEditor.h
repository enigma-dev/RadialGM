#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "BaseEditor.h"
#include "ui_CodeEditor.h"

#include <QLabel>

namespace Ui {
class CodeEditor;
}

class CodeEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit CodeEditor(ProtoModel *model, QWidget *parent);
  ~CodeEditor();

 private slots:
  void updateCursorPositionLabel(int line, int index);
  void updateLineCountLabel(int lines);

 protected:
  Ui::CodeEditor *ui;

 private:
  QLabel *cursorPositionLabel, *lineCountLabel;
};

#endif  // CODEEDITOR_H
