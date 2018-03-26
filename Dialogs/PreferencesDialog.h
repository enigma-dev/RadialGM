#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog {
  Q_OBJECT

 public:
  explicit PreferencesDialog(QWidget *parent);
  ~PreferencesDialog();

 private slots:
  void on_styleCombo_activated(const QString &styleName);

 private:
  Ui::PreferencesDialog *ui;
};

#endif  // PREFERENCESDIALOG_H
