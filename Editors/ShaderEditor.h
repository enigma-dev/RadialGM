#ifndef SHADEREDITOR_H
#define SHADEREDITOR_H

#include "Editors/BaseEditor.h"
#include "Editors/CodeEditor.h"

class ShaderEditor : public BaseEditor {
  Q_OBJECT

 public:
  ShaderEditor(MessageModel* model, QWidget* parent = nullptr);

 private:
  CodeEditor* _codeEditor;
};

#endif  // SHADEREDITOR_H
