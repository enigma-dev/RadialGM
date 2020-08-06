#include "BaseEditor.h"

#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>

BaseEditor::BaseEditor(EditorModel* model, QWidget* parent)
    : QWidget(parent), _model(model) {
  _model->setParent(this);
  _mapper = new EditorMapper(_model, this);
  connect(this, &BaseEditor::FocusGained, [=]() { _hasFocus = true; });
  connect(this, &BaseEditor::FocusLost, [=]() { _hasFocus = false; });
}

void BaseEditor::closeEvent(QCloseEvent* event) {
  //TODO: FIXME
}

bool BaseEditor::HasFocus() { return _hasFocus; }

void BaseEditor::OnSave() {
  //TODO: FIXME
}
