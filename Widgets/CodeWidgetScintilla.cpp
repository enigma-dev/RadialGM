#include "CodeWidget.h"

#include <QFontMetrics>
#include <QLayout>
#include <QPrintDialog>

#include <Qsci/qscilexercpp.h>
#include <Qsci/qsciprinter.h>
#include <Qsci/qsciscintilla.h>

namespace {
QsciLexerCPP* cppLexer = nullptr;
}

CodeWidget::CodeWidget(QWidget* parent) : QWidget(parent), font(QFont("Courier", 10)) {
  QFontMetrics fontMetrics(font);

  if (cppLexer == nullptr) {
    cppLexer = new QsciLexerCPP();
    cppLexer->setFont(font);
  }

  QsciScintilla* codeEdit = new QsciScintilla(this);
  this->textWidget = codeEdit;

  codeEdit->setCaretLineVisible(true);
  codeEdit->setCaretLineBackgroundColor(QColor("#ffe4e4"));

  codeEdit->setMarginLineNumbers(0, true);
  codeEdit->setMarginsFont(font);

  codeEdit->setLexer(cppLexer);

  connect(codeEdit, &QsciScintilla::textChanged, [=]() {
    const int padding = 8;
    auto maxLineString = QString::number(codeEdit->lines());
    codeEdit->setMarginWidth(0, fontMetrics.width(maxLineString) + padding);
  });

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

int CodeWidget::lineCount() { return static_cast<QsciScintilla*>(this->textWidget)->lines(); }

void CodeWidget::gotoLine(int line) { static_cast<QsciScintilla*>(this->textWidget)->setCursorPosition(line - 1, 0); }

void CodeWidget::printSource() {
  QsciPrinter sciPrinter;
  QPrintDialog printDialog(&sciPrinter, this);
  auto codeEdit = static_cast<QsciScintilla*>(this->textWidget);
  if (codeEdit->hasSelectedText()) printDialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);
  if (printDialog.exec() == QDialog::Accepted) this->print(&sciPrinter);
}

void CodeWidget::print(QPrinter* printer) {
  auto sciPrinter = static_cast<QsciPrinter*>(printer);
  auto codeEdit = static_cast<QsciScintilla*>(this->textWidget);
  sciPrinter->printRange(codeEdit);
}
