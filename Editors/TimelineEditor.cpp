#include "TimelineEditor.h"

#include "ui_TimelineEditor.h"

TimelineEditor::TimelineEditor(QWidget* parent, ProtoModel* model)
    : BaseEditor(parent, model), ui(new Ui::TimelineEditor) {
  ui->setupUi(this);
}

TimelineEditor::~TimelineEditor() { delete ui; }
