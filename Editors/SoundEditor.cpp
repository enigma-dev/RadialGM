#include "SoundEditor.h"
#include "ui_SoundEditor.h"

SoundEditor::SoundEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::SoundEditor) {
  ui->setupUi(this);
}

SoundEditor::~SoundEditor() { delete ui; }
