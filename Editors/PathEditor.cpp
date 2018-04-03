#include "PathEditor.h"

#include "ui_PathEditor.h"

PathEditor::PathEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::PathEditor) {
  ui->setupUi(this);
}

PathEditor::~PathEditor() { delete ui; }
