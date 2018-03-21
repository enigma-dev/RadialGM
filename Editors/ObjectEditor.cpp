#include "ObjectEditor.h"

#include "ui_ObjectEditor.h"

ObjectEditor::ObjectEditor(QWidget* parent, ResourceModel* model)
    : BaseEditor(parent, model), ui(new Ui::ObjectEditor) {
  ui->setupUi(this);
}

ObjectEditor::~ObjectEditor() { delete ui; }
