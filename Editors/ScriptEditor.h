#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

#include "Editors/BaseEditor.h"
#include "Editors/CodeEditor.h"

class ScriptEditor : public BaseEditor {
  Q_OBJECT

 public:
  ScriptEditor(MessageModel* model, QWidget* parent = nullptr);

 private:
  CodeEditor* codeEditor;
};

#endif  // SCRIPTEDITOR_H
