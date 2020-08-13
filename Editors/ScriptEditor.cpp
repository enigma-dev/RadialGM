#include "ScriptEditor.h"
#include "ui_CodeEditor.h"

#include "Script.pb.h"

using namespace buffers::resources;

ScriptEditor::ScriptEditor(MessageModel* model, QWidget* parent)
    : BaseEditor(model, parent), _codeEditor(new CodeEditor(this)) {
  this->setWindowIcon(QIcon(":/resources/script.png"));
  QLayout* layout = new QVBoxLayout(this);
  layout->addWidget(_codeEditor);
  layout->setMargin(0);
  setLayout(layout);
  resize(_codeEditor->geometry().width(), _codeEditor->geometry().height());

  Ui::CodeEditor* ui = _codeEditor->_ui;
  connect(ui->actionSave, &QAction::triggered, this, &BaseEditor::OnSave);

  CodeWidget* codeWidget = _codeEditor->AddCodeWidget();

  //TODO: _mapper->mapName(nameEdit);
  _mapper->pushAsset();

  _mapper->mapField(Script::kCodeFieldNumber, codeWidget);

  _mapper->load();

  _codeEditor->updateCursorPositionLabel();
  _codeEditor->updateLineCountLabel();
}
