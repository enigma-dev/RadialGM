#include "RoomEditor.h"
#include "ui_RoomEditor.h"

#include <QFontMetrics>

#ifndef RGM_DISABLE_SYNTAXHIGHLIGHTING
#include <Qsci/qscilexercpp.h>
#include <Qsci/qsciscintilla.h>
#else
#include <QPlainTextEdit>
#endif

RoomEditor::RoomEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::RoomEditor) {
  ui->setupUi(this);

  QFont font("Courier", 10);
  QFontMetrics fontMetrics(font);

#ifndef RGM_DISABLE_SYNTAXHIGHLIGHTING
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
#else
  QPlainTextEdit* plainTextEdit = new QPlainTextEdit(this);
  plainTextEdit->setFont(font);
  this->layout()->addWidget(plainTextEdit);
#endif
}

RoomEditor::~RoomEditor() { delete ui; }
