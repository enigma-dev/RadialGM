#include "CodeEditor.h"

CodeEditor::CodeEditor(ProtoModel *model, QWidget *parent) : BaseEditor(model, parent), ui(new Ui::CodeEditor) {
  ui->setupUi(this);
}

CodeEditor::~CodeEditor() { delete ui; }
