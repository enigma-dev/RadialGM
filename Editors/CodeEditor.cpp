#include "CodeEditor.h"
#include "Widgets/StackedCodeWidget.h"
#include "ui_CodeEditor.h"

CodeEditor::CodeEditor(QWidget* /*parent*/, bool removeSaveBtn) : ui(new Ui::CodeEditor) {
  ui->setupUi(this);

  if (removeSaveBtn) ui->mainToolBar->removeAction(ui->actionSave);

  layout()->setAlignment(Qt::AlignTop);

  connect(ui->actionCut, &QAction::triggered, ui->stackedWidget, &StackedCodeWidget::cut);
  connect(ui->actionCopy, &QAction::triggered, ui->stackedWidget, &StackedCodeWidget::copy);
  connect(ui->actionRedo, &QAction::triggered, ui->stackedWidget, &StackedCodeWidget::redo);
  connect(ui->actionUndo, &QAction::triggered, ui->stackedWidget, &StackedCodeWidget::undo);
  connect(ui->actionPaste, &QAction::triggered, ui->stackedWidget, &StackedCodeWidget::paste);
  connect(ui->actionPrint, &QAction::triggered, ui->stackedWidget, &StackedCodeWidget::printSource);
  connect(ui->actionGoToLine, &QAction::triggered, ui->stackedWidget, &StackedCodeWidget::gotoLineDialog);
  connect(ui->actionNewSource, &QAction::triggered, ui->stackedWidget, &StackedCodeWidget::newSource);
  connect(ui->actionLoadSource, &QAction::triggered, ui->stackedWidget, &StackedCodeWidget::loadSource);
  connect(ui->actionSaveSource, &QAction::triggered, ui->stackedWidget, &StackedCodeWidget::saveSource);
  //connect(ui->actionFindAndReplace, &QAction::triggered, ui->stackedWidget, &StackedCodeWidget::findAndReplace);

  cursorPositionLabel = new QLabel(ui->statusBar);
  lineCountLabel = new QLabel(ui->statusBar);

  connect(ui->stackedWidget, &QStackedWidget::currentChanged, [this]() {
    if (ui->stackedWidget->count() > 0) {
      this->updateCursorPositionLabel();
      this->updateLineCountLabel();
    }
  });

  ui->statusBar->addWidget(cursorPositionLabel);
  ui->statusBar->addWidget(lineCountLabel);
}

CodeEditor::~CodeEditor() { delete ui; }

void CodeEditor::SetDisabled(bool disabled) {
  (disabled) ? ui->stackedWidget->hide() : ui->stackedWidget->show();
  (disabled) ? ui->statusBar->hide() : ui->statusBar->show();
  for (QAction* action : ui->mainToolBar->actions()) {
    action->setDisabled(disabled);
  }
}

int CodeEditor::GetCurrentIndex() { return ui->stackedWidget->currentIndex(); }

void CodeEditor::SetCurrentIndex(int index) { ui->stackedWidget->setCurrentIndex(index); }

void CodeEditor::RemoveCodeWidget(int index) { ui->stackedWidget->removeWidget(ui->stackedWidget->widget(index)); }

void CodeEditor::ClearCodeWidgets() {
  for (int i = 0; i < ui->stackedWidget->count(); ++i) {
    ui->stackedWidget->removeWidget(ui->stackedWidget->widget(i));
  }
}

CodeWidget* CodeEditor::AddCodeWidget() {
  CodeWidget* codeWidget = new CodeWidget(ui->stackedWidget);
  ui->stackedWidget->addWidget(codeWidget);
  connect(codeWidget, &CodeWidget::cursorPositionChanged, this, &CodeEditor::setCursorPositionLabel);
  connect(codeWidget, &CodeWidget::lineCountChanged, this, &CodeEditor::setLineCountLabel);
  return codeWidget;
}

void CodeEditor::setCursorPositionLabel(int line, int index) {
  this->cursorPositionLabel->setText(tr("Ln %0, Col %1").arg(line).arg(index));
}

void CodeEditor::setLineCountLabel(int lines) { this->lineCountLabel->setText(tr("Lines %0").arg(lines)); }

void CodeEditor::updateCursorPositionLabel() {
  auto cursorPosition = static_cast<CodeWidget*>(ui->stackedWidget->currentWidget())->cursorPosition();
  this->setCursorPositionLabel(cursorPosition.first, cursorPosition.second);
}

void CodeEditor::updateLineCountLabel() {
  this->setLineCountLabel(static_cast<CodeWidget*>(ui->stackedWidget->currentWidget())->lineCount());
}
