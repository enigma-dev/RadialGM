#include "ObjectEditor.h"

#include "ui_ObjectEditor.h"

ObjectEditor::ObjectEditor(ProtoModel* model, const QPersistentModelIndex& root, QWidget* parent) :
    BaseEditor(model, root, parent), _ui(new Ui::ObjectEditor) {
  _ui->setupUi(this);
  connect(_ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);
}

ObjectEditor::~ObjectEditor() { delete _ui; }
