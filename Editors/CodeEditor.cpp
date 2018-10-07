#include "CodeEditor.h"
#include "ui_CodeEditor.h"

CodeEditor::CodeEditor(ProtoModel *model, QWidget *parent) : BaseEditor(model, parent), ui(new Ui::CodeEditor) {
  ui->setupUi(this);

  cursorPositionLabel = new QLabel(ui->statusBar);
  lineCountLabel = new QLabel(ui->statusBar);

  ui->statusBar->addWidget(cursorPositionLabel);
  ui->statusBar->addWidget(lineCountLabel);

  // make sure we set the status labels at least once
  updateCursorPositionLabel();
  updateLineCountLabel();

  connect(ui->codeWidget, &CodeWidget::cursorPositionChanged, this, &CodeEditor::setCursorPositionLabel);
  connect(ui->codeWidget, &CodeWidget::lineCountChanged, this, &CodeEditor::setLineCountLabel);
}

CodeEditor::~CodeEditor() { delete ui; }

void CodeEditor::setCursorPositionLabel(int line, int index) {
  this->cursorPositionLabel->setText(tr("Ln %0, Col %1").arg(line).arg(index));
}

void CodeEditor::setLineCountLabel(int lines) { this->lineCountLabel->setText(tr("Lines %0").arg(lines)); }

void CodeEditor::updateCursorPositionLabel() {
  auto cursorPosition = this->ui->codeWidget->cursorPosition();
  this->setCursorPositionLabel(cursorPosition.first, cursorPosition.second);
}

void CodeEditor::updateLineCountLabel() { this->setLineCountLabel(this->ui->codeWidget->lineCount()); }
