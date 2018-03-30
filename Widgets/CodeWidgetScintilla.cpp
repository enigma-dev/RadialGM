#include "CodeWidget.h"

#include <QFontMetrics>
#include <QLayout>

#include <Qsci/qscilexercpp.h>
#include <Qsci/qsciscintilla.h>

CodeWidget::CodeWidget(QWidget* parent) : QWidget(parent), font(QFont("Courier", 10)) {
  QFontMetrics fontMetrics(font);
  QsciScintilla* textEdit = new QsciScintilla(this);

  textEdit->setCaretLineVisible(true);
  textEdit->setCaretLineBackgroundColor(QColor("#ffe4e4"));

  textEdit->setMarginLineNumbers(0, true);
  textEdit->setMarginWidth(0, fontMetrics.width("000"));
  textEdit->setMarginsFont(font);
  QsciLexerCPP* lexer = new QsciLexerCPP();
  lexer->setDefaultFont(font);
  textEdit->setLexer(lexer);
  this->layout()->addWidget(textEdit);
}

CodeWidget::~CodeWidget() {}
