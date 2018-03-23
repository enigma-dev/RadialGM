#include "PathEditor.h"

#include "ui_PathEditor.h"

PathEditor::PathEditor(QWidget* parent, ProtoModel* model) : BaseEditor(parent, model), ui(new Ui::PathEditor) {
  ui->setupUi(this);
}

PathEditor::~PathEditor() { delete ui; }
