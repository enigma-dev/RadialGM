#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QAbstractButton>
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
  void applyClicked();
  void restoreDefaultsClicked();

  void on_PreferencesDialog_accepted();
  void on_PreferencesDialog_rejected();

 private:
  Ui::PreferencesDialog *ui;

  void apply();
  void reset();
};

#endif  // PREFERENCESDIALOG_H
