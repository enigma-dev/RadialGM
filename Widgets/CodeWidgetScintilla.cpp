#include "CodeWidget.h"

#include <QFontMetrics>
#include <QLayout>

#include <Qsci/qscilexercpp.h>
#include <Qsci/qsciscintilla.h>

CodeWidget::CodeWidget(QWidget* parent) : QWidget(parent), font(QFont("Courier", 10)) {
  QFontMetrics fontMetrics(font);
  QsciScintilla* codeEdit = new QsciScintilla(this);
  this->textWidget = codeEdit;

  codeEdit->setCaretLineVisible(true);
  codeEdit->setCaretLineBackgroundColor(QColor("#ffe4e4"));

  codeEdit->setMarginLineNumbers(0, true);
  codeEdit->setMarginWidth(0, fontMetrics.width("000"));
  codeEdit->setMarginsFont(font);
  QsciLexerCPP* lexer = new QsciLexerCPP();
  lexer->setDefaultFont(font);
  codeEdit->setLexer(lexer);

  QVBoxLayout* rootLayout = new QVBoxLayout(this);
  rootLayout->setMargin(0);
  rootLayout->addWidget(codeEdit);
  this->setLayout(rootLayout);
}

CodeWidget::~CodeWidget() {}

QString CodeWidget::code() const { return static_cast<QsciScintilla*>(this->textWidget)->text(); }

void CodeWidget::setCode(QString code) { static_cast<QsciScintilla*>(this->textWidget)->setText(code); }

void CodeWidget::undo() { static_cast<QsciScintilla*>(this->textWidget)->undo(); }

void CodeWidget::redo() { static_cast<QsciScintilla*>(this->textWidget)->redo(); }

void CodeWidget::cut() { static_cast<QsciScintilla*>(this->textWidget)->cut(); }

void CodeWidget::copy() { static_cast<QsciScintilla*>(this->textWidget)->copy(); }

void CodeWidget::paste() { static_cast<QsciScintilla*>(this->textWidget)->paste(); }
