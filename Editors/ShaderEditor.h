#ifndef SHADEREDITOR_H
#define SHADEREDITOR_H

#include "Editors/BaseEditor.h"
#include "Editors/CodeEditor.h"

class ShaderEditor : public BaseEditor {
  Q_OBJECT

public:
  ShaderEditor(ProtoModelPtr model, QWidget *parent = nullptr);
  ~ShaderEditor();

private:
  CodeEditor* codeEditor;
};

#endif  // SHADEREDITOR_H
