#include "ScriptEditor.h"
#include "ui_CodeEditor.h"

#include "codegen/Script.pb.h"

using namespace buffers::resources;

ScriptEditor::ScriptEditor(ProtoModel *model, QWidget *parent) : CodeEditor(model, parent) {
  resMapper->addMapping(ui->codeWidget, Script::kCodeFieldNumber);
  resMapper->toFirst();

  this->updateCursorPositionLabel();
  this->updateLineCountLabel();
}

ScriptEditor::~ScriptEditor() {}
