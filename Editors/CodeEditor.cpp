#include "CodeEditor.h"
#include "Widgets/StackedCodeWidget.h"
#include "ui_CodeEditor.h"

CodeEditor::CodeEditor(QWidget* /*parent*/, bool removeSaveBtn) : _ui(new Ui::CodeEditor) {
  _ui->setupUi(this);

  if (removeSaveBtn) _ui->mainToolBar->removeAction(_ui->actionSave);

  layout()->setAlignment(Qt::AlignTop);

  connect(_ui->actionCut, &QAction::triggered, _ui->stackedWidget, &StackedCodeWidget::cut);
  connect(_ui->actionCopy, &QAction::triggered, _ui->stackedWidget, &StackedCodeWidget::copy);
  connect(_ui->actionRedo, &QAction::triggered, _ui->stackedWidget, &StackedCodeWidget::redo);
  connect(_ui->actionUndo, &QAction::triggered, _ui->stackedWidget, &StackedCodeWidget::undo);
  connect(_ui->actionPaste, &QAction::triggered, _ui->stackedWidget, &StackedCodeWidget::paste);
  connect(_ui->actionPrint, &QAction::triggered, _ui->stackedWidget, &StackedCodeWidget::printSource);
  connect(_ui->actionGoToLine, &QAction::triggered, _ui->stackedWidget, &StackedCodeWidget::gotoLineDialog);
  connect(_ui->actionNewSource, &QAction::triggered, _ui->stackedWidget, &StackedCodeWidget::newSource);
  connect(_ui->actionLoadSource, &QAction::triggered, _ui->stackedWidget, &StackedCodeWidget::loadSource);
  connect(_ui->actionSaveSource, &QAction::triggered, _ui->stackedWidget, &StackedCodeWidget::saveSource);
  //connect(ui->actionFindAndReplace, &QAction::triggered, ui->stackedWidget, &StackedCodeWidget::findAndReplace);

  _cursorPositionLabel = new QLabel(_ui->statusBar);
  _lineCountLabel = new QLabel(_ui->statusBar);

  connect(_ui->stackedWidget, &QStackedWidget::currentChanged, [this]() {
    if (_ui->stackedWidget->count() > 0) {
      this->updateCursorPositionLabel();
      this->updateLineCountLabel();
    }
  });

  _ui->statusBar->addWidget(_cursorPositionLabel);
  _ui->statusBar->addWidget(_lineCountLabel);
}

CodeEditor::~CodeEditor() { delete _ui; }

void CodeEditor::SetDisabled(bool disabled) {
  (disabled) ? _ui->stackedWidget->hide() : _ui->stackedWidget->show();
  (disabled) ? _ui->statusBar->hide() : _ui->statusBar->show();
  auto const actions = _ui->mainToolBar->actions();
  for (QAction* action : actions) {
    action->setDisabled(disabled);
  }
}

int CodeEditor::GetCurrentIndex() { return _ui->stackedWidget->currentIndex(); }

void CodeEditor::SetCurrentIndex(int index) { _ui->stackedWidget->setCurrentIndex(index); }

void CodeEditor::RemoveCodeWidget(int index) { _ui->stackedWidget->removeWidget(_ui->stackedWidget->widget(index)); }

void CodeEditor::ClearCodeWidgets() {
  for (int i = 0; i < _ui->stackedWidget->count(); ++i) {
    _ui->stackedWidget->removeWidget(_ui->stackedWidget->widget(i));
  }
}

CodeWidget* CodeEditor::AddCodeWidget() {
  CodeWidget* codeWidget = new CodeWidget(_ui->stackedWidget);
  _ui->stackedWidget->addWidget(codeWidget);
  connect(codeWidget, &CodeWidget::cursorPositionChanged, this, &CodeEditor::setCursorPositionLabel);
  connect(codeWidget, &CodeWidget::lineCountChanged, this, &CodeEditor::setLineCountLabel);
  return codeWidget;
}

void CodeEditor::setCursorPositionLabel(int line, int index) {
  this->_cursorPositionLabel->setText(tr("Ln %0, Col %1").arg(line).arg(index));
}

void CodeEditor::setLineCountLabel(int lines) { this->_lineCountLabel->setText(tr("Lines %0").arg(lines)); }

void CodeEditor::updateCursorPositionLabel() {
  auto cursorPosition = static_cast<CodeWidget*>(_ui->stackedWidget->currentWidget())->cursorPosition();
  this->setCursorPositionLabel(cursorPosition.first, cursorPosition.second);
}

void CodeEditor::updateLineCountLabel() {
  this->setLineCountLabel(static_cast<CodeWidget*>(_ui->stackedWidget->currentWidget())->lineCount());
}
