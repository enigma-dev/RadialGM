#include "RoomEditor.h"
#include "ui_RoomEditor.h"

RoomEditor::RoomEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::RoomEditor) {
  ui->setupUi(this);
}

RoomEditor::~RoomEditor() { delete ui; }
