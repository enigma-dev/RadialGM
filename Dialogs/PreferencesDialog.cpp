#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include "PreferencesKeys.h"
#include "main.h"

#include <QPushButton>
#include <QSettings>
#include <QStyleFactory>

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PreferencesDialog) {
  ui->setupUi(this);
  this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

  foreach (QString styleName, QStyleFactory::keys()) { ui->styleCombo->addItem(styleName); }

  this->reset();

  connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QAbstractButton::clicked, this,
          &PreferencesDialog::restoreDefaultsClicked);
  connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this,
          &PreferencesDialog::applyClicked);
}

PreferencesDialog::~PreferencesDialog() { delete ui; }

void PreferencesDialog::apply() {
  QSettings settings;
  settings.beginGroup(preferencesKey());

  settings.beginGroup(generalKey());
  settings.setValue(documentationURLKey(), ui->documentationLineEdit->text());
  settings.setValue(websiteURLKey(), ui->websiteLineEdit->text());
  settings.setValue(communityURLKey(), ui->communityLineEdit->text());
  settings.setValue(submitIssueURLKey(), ui->submitIssueLineEdit->text());
  settings.endGroup();  // Preferences/General

  settings.beginGroup(appearanceKey());
  const QString &styleName = ui->styleCombo->currentText();
  settings.setValue(styleNameKey(), styleName);
  QApplication::setStyle(styleName);
  settings.endGroup();  // Preferences/Appearance

  settings.endGroup();  // Preferences
}

void PreferencesDialog::reset() {
  QSettings settings;
  settings.beginGroup(preferencesKey());

  settings.beginGroup(generalKey());
  ui->documentationLineEdit->setText(documentationURL());
  ui->websiteLineEdit->setText(websiteURL());
  ui->communityLineEdit->setText(communityURL());
  ui->submitIssueLineEdit->setText(submitIssueURL());
  settings.endGroup();  // Preferences/General

  settings.endGroup();  // Preferences

  foreach (QString styleName, QStyleFactory::keys()) {
    if (style()->objectName().toLower() == styleName.toLower()) ui->styleCombo->setCurrentText(styleName);
  }
}

void PreferencesDialog::applyClicked() { this->apply(); }

void PreferencesDialog::restoreDefaultsClicked() {
  QSettings settings;
  settings.remove(preferencesKey());

  QApplication::setStyle(defaultStyle);

  this->reset();
}

void PreferencesDialog::on_PreferencesDialog_accepted() { this->apply(); }

void PreferencesDialog::on_PreferencesDialog_rejected() {}
