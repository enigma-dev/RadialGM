#include "EventDialog.h"
#include "ui_EventDialog.h"

EventDialog::EventDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EventDialog)
{
  ui->setupUi(this);
}

EventDialog::~EventDialog()
{
  delete ui;
}
