#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

#include "Editors/BaseEditor.h"
#include "Editors/CodeEditor.h"

class ScriptEditor : public BaseEditor {
  Q_OBJECT

public:
  ScriptEditor(ProtoModelPtr model, QWidget *parent = nullptr);
  ~ScriptEditor();

private:
  CodeEditor* codeEditor;
};

#endif  // SCRIPTEDITOR_H
