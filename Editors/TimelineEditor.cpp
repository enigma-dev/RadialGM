#include "TimelineEditor.h"
#include "ui_TimelineEditor.h"

TimelineEditor::TimelineEditor(QWidget* parent, ResourceModel* /*model*/)
	: QWidget(parent), ui(new Ui::TimelineEditor) {
  ui->setupUi(this);
}

TimelineEditor::~TimelineEditor() { delete ui; }
