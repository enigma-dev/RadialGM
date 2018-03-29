#include "CodeWidget.h"

#include <QPlainTextEdit>

CodeWidget::CodeWidget() : font(QFont("Courier", 10)) {
  QPlainTextEdit* plainTextEdit = new QPlainTextEdit(this);
  plainTextEdit->setFont(font);
  this->layout()->addWidget(plainTextEdit);
}
