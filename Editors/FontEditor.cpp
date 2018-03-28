#include "FontEditor.h"

#include "ui_FontEditor.h"

FontEditor::FontEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::FontEditor) {
  ui->setupUi(this);
}

FontEditor::~FontEditor() { delete ui; }
