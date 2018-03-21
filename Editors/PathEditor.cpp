#include "PathEditor.h"

#include "ui_PathEditor.h"

PathEditor::PathEditor(QWidget* parent, ResourceModel* model) : BaseEditor(parent, model), ui(new Ui::PathEditor) {
  ui->setupUi(this);
}

PathEditor::~PathEditor() { delete ui; }
