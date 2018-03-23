#include "RoomEditor.h"
#include "ui_RoomEditor.h"

#include <Qsci/qscilexercpp.h>
#include <Qsci/qsciscintilla.h>
#include <QFontMetrics>

RoomEditor::RoomEditor(QWidget* parent, ProtoModel* model) : BaseEditor(parent, model), ui(new Ui::RoomEditor) {
  ui->setupUi(this);
  QsciScintilla* textEdit = new QsciScintilla;

  textEdit->setCaretLineVisible(true);
  textEdit->setCaretLineBackgroundColor(QColor("#ffe4e4"));

  textEdit->setMarginLineNumbers(0, true);
  QFont font("Courier", 10);
  QFontMetrics fontMetrics(font);
  textEdit->setMarginWidth(0, fontMetrics.width("000"));
  textEdit->setMarginsFont(font);
  QsciLexerCPP* lexer = new QsciLexerCPP();
  lexer->setDefaultFont(font);
  textEdit->setLexer(lexer);
  this->layout()->addWidget(textEdit);
}

RoomEditor::~RoomEditor() { delete ui; }
