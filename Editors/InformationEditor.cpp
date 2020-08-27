#include "InformationEditor.h"
#include "ui_InformationEditor.h"

#include <QFontComboBox>
#include <QSpinBox>
#include <QFontDialog>
#include <QColorDialog>

InformationEditor::InformationEditor(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::InformationEditor)
{
  ui->setupUi(this);

  // give us a font drop down and size spinner
  auto fontCombo = new QFontComboBox();
  ui->mainToolBar->insertWidget(ui->actionBold, fontCombo);
  auto fontSpinner = new QSpinBox();
  ui->mainToolBar->insertWidget(ui->actionBold, fontSpinner);
  ui->mainToolBar->insertSeparator(ui->actionBold);

  // synchronize the format actions when the selection changes
  auto fontChanged = [=](const QFont &f) {
    fontCombo->setCurrentFont(f);
    fontSpinner->setValue(f.pointSize());
    ui->actionBold->setChecked(f.bold());
    ui->actionItalic->setChecked(f.italic());
    ui->actionUnderline->setChecked(f.underline());
  };
  fontChanged(ui->textEdit->currentFont()); // << initial sync
  connect(ui->textEdit, &QTextEdit::currentCharFormatChanged,
          [fontChanged](const QTextCharFormat &format) {
    fontChanged(format.font());
  });
  // synchronize the alignment group when the selection changes
  auto alignmentChanged = [=](Qt::Alignment a) {
    if (a & Qt::AlignLeft)
        ui->actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        ui->actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        ui->actionAlignRight->setChecked(true);
  };
  alignmentChanged(ui->textEdit->alignment()); // << initial sync
  connect(ui->textEdit, &QTextEdit::cursorPositionChanged,
          [alignmentChanged,this]() {
    alignmentChanged(ui->textEdit->alignment());
    //TODO: bulleted list sync?
  });

  connect(ui->actionPrint, &QAction::triggered, [&](){
    //TODO: Finish me
  });
  connect(ui->actionFont, &QAction::triggered, [&](){
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, ui->textEdit->currentFont(), this);
    if (!ok) return;
    ui->textEdit->setCurrentFont(font);
  });
  connect(fontCombo, &QComboBox::textActivated, ui->textEdit, &QTextEdit::setFontFamily);
  connect(fontSpinner, QOverload<int>::of(&QSpinBox::valueChanged), ui->textEdit,
          &QTextEdit::setFontPointSize);
  connect(ui->actionTextColor, &QAction::triggered, [&](){
    QColor color = QColorDialog::getColor(ui->textEdit->textColor(), this);
    if (!color.isValid()) return;
    ui->textEdit->setTextColor(color);
  });
  connect(ui->actionBold, &QAction::triggered, [&](bool checked){
    ui->textEdit->setFontWeight(checked ? QFont::Bold : QFont::Normal);
  });
  connect(ui->actionAlignGroup, &QActionGroup::triggered, [&](QAction *a){
    if (a == ui->actionAlignLeft)
        ui->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == ui->actionAlignCenter)
        ui->textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == ui->actionAlignRight)
        ui->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
  });
  connect(ui->actionBulletedList, &QAction::triggered, [&](bool checked){
    //TODO: Finish me
  });
}

InformationEditor::~InformationEditor() {
  delete ui;
}

