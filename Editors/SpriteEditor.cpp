#include "SpriteEditor.h"

#include "ui_SpriteEditor.h"

SpriteEditor::SpriteEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::SpriteEditor) {
  ui->setupUi(this);
}

SpriteEditor::~SpriteEditor() { delete ui; }
