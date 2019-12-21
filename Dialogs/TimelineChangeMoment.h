#ifndef TIMELINECHANGEMOMENT_H
#define TIMELINECHANGEMOMENT_H

#include <QDialog>

namespace Ui {
class TimelineChangeMoment;
}

class TimelineChangeMoment : public QDialog
{
    Q_OBJECT

public:
    explicit TimelineChangeMoment(QWidget *parent = nullptr);
    ~TimelineChangeMoment();
    int GetSpinBoxValue();
    void SetSpinBoxValue(int val);

private:
    Ui::TimelineChangeMoment *ui;
};

#endif // TIMELINECHANGEMOMENT_H
