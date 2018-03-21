#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <QStyleFactory>

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PreferencesDialog) {
  ui->setupUi(this);
  this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

  foreach (QString styleName, QStyleFactory::keys()) {
	ui->styleCombo->addItem(styleName);
	if (style()->objectName().toLower() == styleName.toLower())
	  ui->styleCombo->setCurrentIndex(ui->styleCombo->count() - 1);
  }
}

PreferencesDialog::~PreferencesDialog() { delete ui; }

void PreferencesDialog::on_styleCombo_activated(const QString &styleName) {
  QStyle *style = QStyleFactory::create(styleName);
  if (style) QApplication::setStyle(style);
}
