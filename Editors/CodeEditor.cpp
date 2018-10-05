#include "CodeEditor.h"
CodeEditor::CodeEditor(ProtoModel *model, QWidget *parent) : BaseEditor(model, parent), ui(new Ui::CodeEditor) {
  ui->setupUi(this);

  cursorPositionLabel = new QLabel(ui->statusBar);
  lineCountLabel = new QLabel(ui->statusBar);

  ui->statusBar->addWidget(cursorPositionLabel);
  ui->statusBar->addWidget(lineCountLabel);

  // make sure we set the status labels at least once
  updateCursorPositionLabel(1, 1);
  updateLineCountLabel(1);

  connect(ui->codeWidget, &CodeWidget::cursorPositionChanged, this, &CodeEditor::updateCursorPositionLabel);
  connect(ui->codeWidget, &CodeWidget::lineCountChanged, this, &CodeEditor::updateLineCountLabel);
}

CodeEditor::~CodeEditor() { delete ui; }

void CodeEditor::updateCursorPositionLabel(int line, int index) {
  this->cursorPositionLabel->setText(tr("Ln %0, Col %1").arg(line).arg(index));
}

void CodeEditor::updateLineCountLabel(int lines) { this->lineCountLabel->setText(tr("Lines %0").arg(lines)); }
