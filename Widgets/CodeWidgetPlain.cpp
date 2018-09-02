#include "CodeWidget.h"

#include <QLayout>
#include <QPlainTextEdit>

CodeWidget::CodeWidget(QWidget* parent) : QWidget(parent), font(QFont("Courier", 10)) {
  QPlainTextEdit* plainTextEdit = new QPlainTextEdit(this);
  plainTextEdit->setFont(font);
  this->layout()->addWidget(plainTextEdit);
}

CodeWidget::~CodeWidget() {}
