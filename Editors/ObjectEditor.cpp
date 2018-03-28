#include "ObjectEditor.h"

#include "ui_ObjectEditor.h"

ObjectEditor::ObjectEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::ObjectEditor) {
  ui->setupUi(this);
}

ObjectEditor::~ObjectEditor() { delete ui; }
