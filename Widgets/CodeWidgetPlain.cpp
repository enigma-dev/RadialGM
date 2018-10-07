#include "CodeWidget.h"

#include <QLayout>
#include <QPlainTextEdit>
#include <QPrintDialog>
#include <QTextBlock>
#include <QTextCursor>

CodeWidget::CodeWidget(QWidget* parent) : QWidget(parent), font(QFont("Courier", 10)) {
  QPlainTextEdit* plainTextEdit = new QPlainTextEdit(this);
  this->textWidget = plainTextEdit;
  plainTextEdit->setFont(font);

  connect(plainTextEdit, &QPlainTextEdit::textChanged, this, &CodeWidget::codeChanged);
  connect(plainTextEdit, &QPlainTextEdit::blockCountChanged, this, &CodeWidget::lineCountChanged);

  connect(plainTextEdit, &QPlainTextEdit::cursorPositionChanged, [=]() {
    QTextCursor cursor = plainTextEdit->textCursor();
    emit cursorPositionChanged(cursor.blockNumber() + 1, cursor.positionInBlock() + 1);
  });

  QVBoxLayout* rootLayout = new QVBoxLayout(this);
  rootLayout->setMargin(0);
  rootLayout->addWidget(plainTextEdit);
  this->setLayout(rootLayout);
}

CodeWidget::~CodeWidget() {}

QString CodeWidget::code() const { return static_cast<QPlainTextEdit*>(this->textWidget)->toPlainText(); }

void CodeWidget::setCode(QString code) { static_cast<QPlainTextEdit*>(this->textWidget)->setPlainText(code); }

void CodeWidget::undo() { static_cast<QPlainTextEdit*>(this->textWidget)->undo(); }

void CodeWidget::redo() { static_cast<QPlainTextEdit*>(this->textWidget)->redo(); }

void CodeWidget::cut() { static_cast<QPlainTextEdit*>(this->textWidget)->cut(); }

void CodeWidget::copy() { static_cast<QPlainTextEdit*>(this->textWidget)->copy(); }

void CodeWidget::paste() { static_cast<QPlainTextEdit*>(this->textWidget)->paste(); }

int CodeWidget::lineCount() { return static_cast<QPlainTextEdit*>(this->textWidget)->blockCount(); }

QPair<int, int> CodeWidget::cursorPosition() {
  auto plainTextEdit = static_cast<QPlainTextEdit*>(this->textWidget);
  auto cursor = plainTextEdit->textCursor();
  return QPair<int, int>(cursor.blockNumber() + 1, cursor.positionInBlock() + 1);
}

void CodeWidget::gotoLine(int line) {
  auto plainTextEdit = static_cast<QPlainTextEdit*>(this->textWidget);
  QTextCursor textCursor(plainTextEdit->document()->findBlockByLineNumber(line - 1));
  plainTextEdit->setTextCursor(textCursor);
}

void CodeWidget::printSource() {
  QPrinter printer;
  QPrintDialog printDialog(&printer, this);
  auto plainTextEdit = static_cast<QPlainTextEdit*>(this->textWidget);
  if (plainTextEdit->textCursor().hasSelection()) printDialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);
  if (printDialog.exec() == QDialog::Accepted) this->print(&printer);
}

void CodeWidget::print(QPrinter* printer) {
  auto plainTextEdit = static_cast<QPlainTextEdit*>(this->textWidget);
  plainTextEdit->document()->print(printer);
}
