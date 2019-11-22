#include "FontEditor.h"

#include "ui_FontEditor.h"

FontEditor::FontEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::FontEditor) {
  ui->setupUi(this);
  connect(ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);
}

FontEditor::~FontEditor() { delete ui; }
