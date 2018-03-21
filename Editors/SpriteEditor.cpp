#include "SpriteEditor.h"

#include "ui_SpriteEditor.h"

SpriteEditor::SpriteEditor(QWidget* parent, ResourceModel* model)
    : BaseEditor(parent, model), ui(new Ui::SpriteEditor) {
  ui->setupUi(this);
}

SpriteEditor::~SpriteEditor() { delete ui; }
