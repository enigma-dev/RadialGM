#ifndef EVENTARGUMENTSDIALOG_H
#define EVENTARGUMENTSDIALOG_H

#include "event_reader/event_parser.h"

#include <QDialog>

class EventArgumentsDialog : public QDialog
{
  Q_OBJECT
public:
  EventArgumentsDialog(QWidget* parent, const QStringList& arguments);
  QSize sizeHint() const override;
  const QStringList& GetArguments() const;
  void done(int r) override;

private:
  QStringList arguments_;
  QVector<QWidget*> widgets_;
};

#endif // EVENTARGUMENTSDIALOG_H
