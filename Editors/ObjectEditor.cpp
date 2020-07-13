#include "ObjectEditor.h"
#include "Dialogs/EventDialog.h"

#include "ui_ObjectEditor.h"

ObjectEditor::ObjectEditor(MessageModel* model, QWidget* parent) : BaseEditor(model, parent), _ui(new Ui::ObjectEditor) {
  _ui->setupUi(this);
  connect(_ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);
}

ObjectEditor::~ObjectEditor() { delete _ui; }

void ObjectEditor::on_actionAddEvent_triggered() {
  auto evts = new EventDialog(this);
  evts->show();
}
