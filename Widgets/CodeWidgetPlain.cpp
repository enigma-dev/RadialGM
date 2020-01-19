#include "CodeWidget.h"

#include <QLayout>
#include <QPlainTextEdit>
#include <QPrintDialog>
#include <QTextBlock>
#include <QTextCursor>

void CodeWidget::prepareKeywordStore() {}
void CodeWidget::addKeyword(const QString& /*keyword*/, KeywordType /*type*/) {}
void CodeWidget::addCalltip(const QString& /*keyword*/, const QString& /*calltip*/, KeywordType /*type*/) {}
void CodeWidget::finalizeKeywords() {}

CodeWidget::CodeWidget(QWidget* parent) : QWidget(parent), _font(QFont("Courier", 10)) {
  QPlainTextEdit* plainTextEdit = new QPlainTextEdit(this);
  this->_textWidget = plainTextEdit;
  plainTextEdit->setFont(_font);

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

QString CodeWidget::code() const { return static_cast<QPlainTextEdit*>(this->_textWidget)->toPlainText(); }

void CodeWidget::setCode(QString code) { static_cast<QPlainTextEdit*>(this->_textWidget)->setPlainText(code); }

void CodeWidget::undo() { static_cast<QPlainTextEdit*>(this->_textWidget)->undo(); }

void CodeWidget::redo() { static_cast<QPlainTextEdit*>(this->_textWidget)->redo(); }

void CodeWidget::cut() { static_cast<QPlainTextEdit*>(this->_textWidget)->cut(); }

void CodeWidget::copy() { static_cast<QPlainTextEdit*>(this->_textWidget)->copy(); }

void CodeWidget::paste() { static_cast<QPlainTextEdit*>(this->_textWidget)->paste(); }

int CodeWidget::lineCount() { return static_cast<QPlainTextEdit*>(this->_textWidget)->blockCount(); }

QPair<int, int> CodeWidget::cursorPosition() {
  auto plainTextEdit = static_cast<QPlainTextEdit*>(this->_textWidget);
  auto cursor = plainTextEdit->textCursor();
  return QPair<int, int>(cursor.blockNumber() + 1, cursor.positionInBlock() + 1);
}

void CodeWidget::gotoLine(int line) {
  auto plainTextEdit = static_cast<QPlainTextEdit*>(this->_textWidget);
  QTextCursor textCursor(plainTextEdit->document()->findBlockByLineNumber(line - 1));
  plainTextEdit->setTextCursor(textCursor);
}

void CodeWidget::printSource() {
  QPrinter printer;
  QPrintDialog printDialog(&printer, this);
  auto plainTextEdit = static_cast<QPlainTextEdit*>(this->_textWidget);
  if (plainTextEdit->textCursor().hasSelection()) printDialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);
  if (printDialog.exec() == QDialog::Accepted) this->print(&printer);
}

void CodeWidget::print(QPrinter* printer) {
  auto plainTextEdit = static_cast<QPlainTextEdit*>(this->_textWidget);
  plainTextEdit->document()->print(printer);
}
