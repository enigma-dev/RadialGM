#include "TimelineChangeMoment.h"
#include "ui_TimelineChangeMoment.h"

TimelineChangeMoment::TimelineChangeMoment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimelineChangeMoment) {
    ui->setupUi(this);
}

TimelineChangeMoment::~TimelineChangeMoment() { delete ui; }

int TimelineChangeMoment::GetSpinBoxValue() {
  return ui->spinBox->value();
}

void TimelineChangeMoment::SetSpinBoxValue(int val) {
  ui->spinBox->setValue(val);
}
