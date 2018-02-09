#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <QStyleFactory>
#include <QDebug>
PreferencesDialog::PreferencesDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PreferencesDialog)
{
	ui->setupUi(this);

	foreach (QString styleName, QStyleFactory::keys()) {
		qDebug() << styleName;
		ui->styleCombo->addItem(styleName);
		if (style()->objectName().toLower() == styleName.toLower())
			ui->styleCombo->setCurrentIndex(ui->styleCombo->count() - 1);
	}
}

PreferencesDialog::~PreferencesDialog()
{
	delete ui;
}

void PreferencesDialog::on_styleCombo_activated(const QString &styleName)
{
	QStyle *style = QStyleFactory::create(styleName);
	if (style)
		QApplication::setStyle(style);
}
