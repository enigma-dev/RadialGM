#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "Widgets/CodeWidget.h"

#include <QLabel>

namespace Ui {
class CodeEditor;
}

class CodeEditor : public QWidget {
  Q_OBJECT

 public:
  explicit CodeEditor(QWidget *parent, bool removeSaveBtn = false);
  ~CodeEditor();
  void SetDisabled(bool disabled);
  CodeWidget *AddCodeWidget();
  int GetCurrentIndex();
  void SetCurrentIndex(int index);
  void RemoveCodeWidget(int index);
  void ClearCodeWidgets();
  Ui::CodeEditor *_ui;

 public slots:
  void setCursorPositionLabel(int line, int index);
  void setLineCountLabel(int lines);
  void updateCursorPositionLabel();
  void updateLineCountLabel();

 private:
  QLabel *_cursorPositionLabel, *_lineCountLabel;
};

#endif  // CODEEDITOR_H
