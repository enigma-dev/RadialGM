#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

#include "Editors/CodeEditor.h"

class ScriptEditor : public CodeEditor {
  Q_OBJECT

 public:
  ScriptEditor(ProtoModel *model, QWidget *parent = nullptr);
  ~ScriptEditor();
};

#endif  // SCRIPTEDITOR_H
