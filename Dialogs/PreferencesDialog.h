#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include <QSettings>
#include <QJsonObject>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog {
  Q_OBJECT

 public:
  explicit PreferencesDialog(QWidget *parent);
  ~PreferencesDialog();

  static QJsonObject groupToJson(const QSettings& settings);
  static void groupFromJson(QSettings& settings, QJsonObject obj);

 private slots:
  void applyClicked();
  void restoreDefaultsClicked();

  void on_PreferencesDialog_accepted();
  void on_PreferencesDialog_rejected();

 private:
  Ui::PreferencesDialog *ui;

  void setupKeybindingUI();
  void setupKeybindingContextUI();
  void apply();
  void reset();
};

#endif  // PREFERENCESDIALOG_H
