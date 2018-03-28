#include "TimelineEditor.h"

#include "ui_TimelineEditor.h"

TimelineEditor::TimelineEditor(ProtoModel* model, QWidget* parent)
    : BaseEditor(model, parent), ui(new Ui::TimelineEditor) {
  ui->setupUi(this);
}

TimelineEditor::~TimelineEditor() { delete ui; }
