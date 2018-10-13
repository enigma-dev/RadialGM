#include "CodeWidget.h"
#include "Components/ArtManager.h"

#include <Qsci/qsciapis.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qsciprinter.h>
#include <Qsci/qsciscintilla.h>

#include <QFontMetrics>
#include <QLayout>
#include <QPrintDialog>
#include <QShortcut>

namespace {

QsciLexerCPP* cppLexer = nullptr;
QsciAPIs* sciApis = nullptr;

void prepare_scintilla_apis() {
  if (cppLexer == nullptr) {
    cppLexer = new QsciLexerCPP();
    cppLexer->setFont(QFont("Courier", 10));
  }

  if (sciApis == nullptr) {
    sciApis = new QsciAPIs(cppLexer);
  }
}

}  // anonymous namespace

void CodeWidget::prepareKeywordStore() { prepare_scintilla_apis(); }

void CodeWidget::addKeyword(const QString& keyword) { sciApis->add(keyword); }

void CodeWidget::finalizeKeywords() { sciApis->prepare(); }

CodeWidget::CodeWidget(QWidget* parent) : QWidget(parent), font(QFont("Courier", 10)) {
  prepare_scintilla_apis();

  QFontMetrics fontMetrics(font);

  QsciScintilla* codeEdit = new QsciScintilla(this);
  this->textWidget = codeEdit;
  codeEdit->registerImage(0, QPixmap(":/actions/right-arrow-red.png"));
  codeEdit->registerImage(1, QPixmap(":/actions/right-arrow-blue.png"));
  codeEdit->registerImage(2, QPixmap(":/actions/right-arrow-green.png"));
  codeEdit->registerImage(3, QPixmap(":/actions/function.png"));
  codeEdit->registerImage(4, ArtManager::GetIcon("object").pixmap(18, 18));
  codeEdit->registerImage(5, ArtManager::GetIcon("sprite").pixmap(18, 18));

  QShortcut* shortcut = new QShortcut(codeEdit);
  shortcut->setKey(QKeySequence(Qt::CTRL + Qt::Key_Space));
  connect(shortcut, &QShortcut::activated, codeEdit, &QsciScintilla::autoCompleteFromAll);
  shortcut->setAutoRepeat(false);
  codeEdit->setAutoCompletionThreshold(2);
  codeEdit->setAutoCompletionSource(QsciScintilla::AutoCompletionSource::AcsAPIs);

  codeEdit->setCaretLineVisible(true);
  codeEdit->setCaretLineBackgroundColor(QColor("#ffe4e4"));

  codeEdit->setMarginLineNumbers(0, true);
  codeEdit->setMarginsFont(font);

  codeEdit->setLexer(cppLexer);

  connect(codeEdit, &QsciScintilla::textChanged, this, &CodeWidget::codeChanged);

  connect(codeEdit, &QsciScintilla::linesChanged, [=]() {
    const int padding = 8;
    auto maxLineString = QString::number(codeEdit->lines());
    codeEdit->setMarginWidth(0, fontMetrics.width(maxLineString) + padding);
    emit lineCountChanged(codeEdit->lines());
  });

  connect(codeEdit, &QsciScintilla::cursorPositionChanged,
          [=](int line, int index) { emit cursorPositionChanged(line + 1, index + 1); });

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

QPair<int, int> CodeWidget::cursorPosition() {
  auto sci = static_cast<QsciScintilla*>(this->textWidget);
  int line, index;
  sci->getCursorPosition(&line, &index);
  return QPair<int, int>(line + 1, index + 1);
}

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
