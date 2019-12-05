#include "ObjectEditor.h"

#include "ui_ObjectEditor.h"

ObjectEditor::ObjectEditor(ProtoModelPtr model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::ObjectEditor) {
  ui->setupUi(this);
  connect(ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);
}

ObjectEditor::~ObjectEditor() { delete ui; }
