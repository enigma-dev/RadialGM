#include "IncludeEditor.h"
#include "ui_IncludeEditor.h"

IncludeEditor::IncludeEditor(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::IncludeEditor)
{
  ui->setupUi(this);
}

IncludeEditor::~IncludeEditor()
{
  delete ui;
}
