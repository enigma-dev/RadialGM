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
}

InformationEditor::~InformationEditor()
{
  delete ui;
}
