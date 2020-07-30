#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

#include "Editors/BaseEditor.h"
#include "Editors/CodeEditor.h"

class ScriptEditor : public BaseEditor {
  Q_OBJECT

 public:
  ScriptEditor(ProtoModel* model, const QPersistentModelIndex& root, QWidget* parent = nullptr);

 private:
  CodeEditor* _codeEditor;
};

#endif  // SCRIPTEDITOR_H
