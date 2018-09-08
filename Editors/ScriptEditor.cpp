#include "ScriptEditor.h"

#include "codegen/Script.pb.h"

using namespace buffers::resources;

ScriptEditor::ScriptEditor(ProtoModel *model, QWidget *parent) : CodeEditor(model, parent) {
  mapper->addMapping(ui->codeWidget, Script::kCodeFieldNumber);
  mapper->toFirst();
  //ui->codeWidget->setCode()
}

ScriptEditor::~ScriptEditor() {}
