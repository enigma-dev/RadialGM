#include "CodeWidget.h"

#include <QLayout>
#include <QPlainTextEdit>

CodeWidget::CodeWidget(QWidget* parent) : QWidget(parent), font(QFont("Courier", 10)) {
  QPlainTextEdit* plainTextEdit = new QPlainTextEdit(this);
  this->textWidget = plainTextEdit;
  plainTextEdit->setFont(font);

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
