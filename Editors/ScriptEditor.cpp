#include "ScriptEditor.h"
#include "ui_CodeEditor.h"

#include "Script.pb.h"

using namespace buffers::resources;

ScriptEditor::ScriptEditor(MessageModel* model, QWidget* parent)
    : BaseEditor(model, parent), codeEditor(new CodeEditor(this)) {
  QLayout* layout = new QVBoxLayout(this);
  layout->addWidget(codeEditor);
  layout->setMargin(0);
  setLayout(layout);
  resize(codeEditor->geometry().width(), codeEditor->geometry().height());

  Ui::CodeEditor* ui = codeEditor->ui;
  connect(ui->actionSave, &QAction::triggered, this, &BaseEditor::OnSave);

  CodeWidget* codeWidget = codeEditor->AddCodeWidget();
  resMapper->addMapping(codeWidget, Script::kCodeFieldNumber);
  resMapper->toFirst();

  codeEditor->updateCursorPositionLabel();
  codeEditor->updateLineCountLabel();
}
