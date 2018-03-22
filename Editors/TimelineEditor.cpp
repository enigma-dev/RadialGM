#include "TimelineEditor.h"

#include "ui_TimelineEditor.h"

TimelineEditor::TimelineEditor(QWidget* parent, ResourceModel* model)
    : BaseEditor(parent, model), ui(new Ui::TimelineEditor) {
  ui->setupUi(this);
}

TimelineEditor::~TimelineEditor() { delete ui; }
