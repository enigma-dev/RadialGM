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
