#include "FontEditor.h"

#include "ui_FontEditor.h"

FontEditor::FontEditor(QWidget* parent, ResourceModel* model) : BaseEditor(parent, model), ui(new Ui::FontEditor) {
  ui->setupUi(this);
}

FontEditor::~FontEditor() { delete ui; }
