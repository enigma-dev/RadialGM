#include "InformationEditor.h"
#include "ui_InformationEditor.h"

InformationEditor::InformationEditor(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::InformationEditor)
{
  ui->setupUi(this);
}

InformationEditor::~InformationEditor()
{
  delete ui;
}
