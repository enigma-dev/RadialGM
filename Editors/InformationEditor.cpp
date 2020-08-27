#include "InformationEditor.h"
#include "ui_InformationEditor.h"

#include <QFontComboBox>
#include <QSpinBox>

InformationEditor::InformationEditor(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::InformationEditor)
{
  ui->setupUi(this);

  auto fontCombo = new QFontComboBox();
  ui->mainToolBar->insertWidget(ui->actionBold, fontCombo);
  auto fontSpinner = new QSpinBox();
  ui->mainToolBar->insertWidget(ui->actionBold, fontSpinner);
  ui->mainToolBar->insertSeparator(ui->actionBold);

  connect(ui->actionBold, &QAction::triggered, [&](bool checked){
    ui->textEdit->setFontWeight(checked ? QFont::Bold : QFont::Normal);
  });
  connect(ui->actionBulletedList, &QAction::triggered, [&](bool checked){
    //TODO: Finish me
  });
  connect(ui->actionAlignGroup, &QActionGroup::triggered, [&](QAction *a){
    if (a == ui->actionAlignLeft)
        ui->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == ui->actionAlignCenter)
        ui->textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == ui->actionAlignRight)
        ui->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
  });
  connect(fontCombo, &QComboBox::textActivated, ui->textEdit, &QTextEdit::setFontFamily);
  connect(fontSpinner, QOverload<int>::of(&QSpinBox::valueChanged), ui->textEdit,
          &QTextEdit::setFontPointSize);
}

InformationEditor::~InformationEditor()
{
  delete ui;
}
