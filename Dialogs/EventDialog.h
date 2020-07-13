#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>

namespace Ui {
class EventDialog;
}

class EventDialog : public QDialog
{
  Q_OBJECT

public:
  explicit EventDialog(QWidget *parent = nullptr);
  ~EventDialog();

private:
  Ui::EventDialog *ui;
};

#endif // EVENTDIALOG_H
