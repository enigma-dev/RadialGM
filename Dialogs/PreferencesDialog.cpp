#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

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

static inline QString preferencesKey() { return QStringLiteral("Preferences"); }
static inline QString generalKey() { return QStringLiteral("General"); }
static inline QString appearanceKey() { return QStringLiteral("Appearance"); }

void PreferencesDialog::apply() {
  QSettings settings;
  settings.beginGroup(preferencesKey());

  settings.beginGroup(generalKey());
  settings.setValue("documentationURI", ui->documentationLineEdit->text());
  settings.setValue("websiteURI", ui->websiteLineEdit->text());
  settings.setValue("communityURI", ui->communityLineEdit->text());
  settings.setValue("submitIssueURI", ui->submitIssueLineEdit->text());
  settings.endGroup();  // Preferences/General

  settings.beginGroup(appearanceKey());
  const QString &styleName = ui->styleCombo->currentText();
  settings.setValue("styleName", styleName);
  QStyle *style = QStyleFactory::create(styleName);
  if (style) QApplication::setStyle(style);
  settings.endGroup();  // Preferences/Appearance

  settings.endGroup();  // Preferences
}

void PreferencesDialog::reset() {
  QSettings settings;
  settings.beginGroup(preferencesKey());

  settings.beginGroup(generalKey());
  ui->documentationLineEdit->setText(
      settings.value("documentationURI", "https://enigma-dev.org/docs/Wiki/Main_Page").toString());
  ui->websiteLineEdit->setText(settings.value("websiteURI", "https://enigma-dev.org").toString());
  ui->communityLineEdit->setText(settings.value("communityURI", "https://enigma-dev.org/forums/").toString());
  ui->submitIssueLineEdit->setText(
      settings.value("submitIssueURI", "https://github.com/enigma-dev/RadialGM/issues").toString());
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
