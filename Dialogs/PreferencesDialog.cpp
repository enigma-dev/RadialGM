#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include "PreferencesKeys.h"
#include "main.h"

#include "ui_MainWindow.h"
#include "ui_SpriteEditor.h"
#include "ui_SoundEditor.h"

#include <QKeySequenceEdit>
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

template <typename T, typename P = QWidget>
P *KeybindingFactory() {
  P* context = new P();
  T *lol = new T();
  lol->setupUi(context);
  return context;
}

static QList<QPair<QString,std::function<QWidget*()>>> keybindingFactories = {
  {QObject::tr("Global"),KeybindingFactory<Ui::MainWindow,QMainWindow>},
  {QObject::tr("Sprite Editor"),KeybindingFactory<Ui::SpriteEditor>},
  {QObject::tr("Sound Editor"),KeybindingFactory<Ui::SoundEditor>}
};

void PreferencesDialog::on_keybindingList_currentRowChanged(int row) {
  auto keybindingFactory = keybindingFactories[0];
  std::function<QWidget*()> factoryFunction = keybindingFactory.second;
  QWidget *widget = factoryFunction();
  QList<QAction*> actions = widget->findChildren<QAction*>(QString(),Qt::FindDirectChildrenOnly);
  ui->keybindingTable->clearContents();
  ui->keybindingTable->setRowCount(actions.size());
  for (int i = 0; i < actions.size(); ++i) {
    auto action = actions[i];
    QHBoxLayout *hLayout = new QHBoxLayout();
    auto shortcutValue = new QKeySequenceEdit(action->shortcut());
    QLabel *item = new QLabel(action->text());
    item->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    QLabel *item2 = new QLabel();
    item2->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    item->setBuddy(shortcutValue);
    item2->setPixmap(action->icon().pixmap(16));

   // QFont boldFont = item->font();
    //boldFont.setItalic(true);
    //item->setFont(boldFont);
    hLayout->addWidget(item2);
    hLayout->addWidget(item);
    QWidget* actionWidget = new QWidget();
    actionWidget->setLayout(hLayout);
    ui->keybindingTable->setCellWidget(i, 0, actionWidget);
    ui->keybindingTable->setCellWidget(i, 1, shortcutValue);
  }
}
