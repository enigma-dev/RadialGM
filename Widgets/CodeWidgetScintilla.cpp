#include "CodeWidget.h"
#include "Components/ArtManager.h"
#include "Models/TreeModel.h"

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

void CodeWidget::prepareKeywordStore() {
  if (sciApis != nullptr) {
    delete sciApis;
    sciApis = nullptr;
  }
  prepare_scintilla_apis();
}

void CodeWidget::addKeyword(const QString& keyword, KeywordType type) {
  if (!sciApis) return;
  QString fmt = keyword + QObject::tr("?%0").arg(type);
  sciApis->add(fmt);
}

void CodeWidget::addCalltip(const QString& keyword, const QString& calltip, KeywordType type) {
  if (!sciApis) return;
  QString fmt = keyword + QObject::tr("?%0(%1)").arg(type).arg(calltip);
  sciApis->add(fmt);
}

void CodeWidget::finalizeKeywords() {
  if (!sciApis) return;
  sciApis->prepare();
}

CodeWidget::CodeWidget(QWidget* parent) : QWidget(parent), _font(QFont("Courier", 10)) {
  prepare_scintilla_apis();

  QFontMetrics fontMetrics(_font);

  QsciScintilla* codeEdit = new QsciScintilla(this);
  this->_textWidget = codeEdit;
  codeEdit->registerImage(KeywordType::UNKNOWN, QPixmap(":/actions/right-arrow-red.png"));
  codeEdit->registerImage(KeywordType::TYPE_NAME, QPixmap(":/actions/right-arrow-blue.png"));
  codeEdit->registerImage(KeywordType::GLOBAL, QPixmap(":/actions/right-arrow-green.png"));
  codeEdit->registerImage(KeywordType::FUNCTION, QPixmap(":/actions/function.png"));

  // FIXME: this was moved / renamed
  /*for (auto type : TreeModel::iconMap) {
    codeEdit->registerImage(KeywordType::MAX + type.first, type.second.pixmap(18, 18));
  }*/

  QShortcut* shortcut = new QShortcut(codeEdit);
  shortcut->setKey(QKeySequence(Qt::CTRL + Qt::Key_Space));
  connect(shortcut, &QShortcut::activated, codeEdit, &QsciScintilla::autoCompleteFromAll);
  shortcut->setAutoRepeat(false);
  codeEdit->setAutoCompletionThreshold(2);
  codeEdit->setAutoCompletionSource(QsciScintilla::AutoCompletionSource::AcsAll);

  codeEdit->setCaretLineVisible(true);
  codeEdit->setCaretLineBackgroundColor(QColor("#ffe4e4"));

  codeEdit->setMarginLineNumbers(0, true);
  codeEdit->setMarginsFont(_font);

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

QString CodeWidget::code() const { return static_cast<QsciScintilla*>(this->_textWidget)->text(); }

void CodeWidget::setCode(QString code) { static_cast<QsciScintilla*>(this->_textWidget)->setText(code); }

void CodeWidget::undo() { static_cast<QsciScintilla*>(this->_textWidget)->undo(); }

void CodeWidget::redo() { static_cast<QsciScintilla*>(this->_textWidget)->redo(); }

void CodeWidget::cut() { static_cast<QsciScintilla*>(this->_textWidget)->cut(); }

void CodeWidget::copy() { static_cast<QsciScintilla*>(this->_textWidget)->copy(); }

void CodeWidget::paste() { static_cast<QsciScintilla*>(this->_textWidget)->paste(); }

int CodeWidget::lineCount() { return static_cast<QsciScintilla*>(this->_textWidget)->lines(); }

QPair<int, int> CodeWidget::cursorPosition() {
  auto sci = static_cast<QsciScintilla*>(this->_textWidget);
  int line, index;
  sci->getCursorPosition(&line, &index);
  return QPair<int, int>(line + 1, index + 1);
}

void CodeWidget::gotoLine(int line) { static_cast<QsciScintilla*>(this->_textWidget)->setCursorPosition(line - 1, 0); }

void CodeWidget::printSource() {
  QsciPrinter sciPrinter;
  QPrintDialog printDialog(&sciPrinter, this);
  auto codeEdit = static_cast<QsciScintilla*>(this->_textWidget);
  if (codeEdit->hasSelectedText()) printDialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);
  if (printDialog.exec() == QDialog::Accepted) this->print(&sciPrinter);
}

void CodeWidget::print(QPrinter* printer) {
  auto sciPrinter = static_cast<QsciPrinter*>(printer);
  auto codeEdit = static_cast<QsciScintilla*>(this->_textWidget);
  sciPrinter->printRange(codeEdit);
}
