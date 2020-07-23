#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include "PreferencesKeys.h"
#include "KeybindingPreferences.h"
#include "main.h"
#include "Components/Logger.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QKeySequenceEdit>
#include <QPushButton>
#include <QStyleFactory>

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PreferencesDialog) {
  ui->setupUi(this);
  this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

  foreach (QString styleName, QStyleFactory::keys()) { ui->styleCombo->addItem(styleName); }
  this->setupKeybindingUI();
  this->reset();

  connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QAbstractButton::clicked, this,
          &PreferencesDialog::restoreDefaultsClicked);
  connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this,
          &PreferencesDialog::applyClicked);
}

PreferencesDialog::~PreferencesDialog() { delete ui; }

QJsonObject PreferencesDialog::groupToJson(const QSettings& settings) {
  QJsonObject json;
  QStringList ctxGroups = settings.childGroups();
  for (QString ctx : ctxGroups) {
    QJsonObject ctxJson;
    QStringList keybindings = settings.childGroups();
    for (QString keybinding : keybindings)
      ctxJson[keybinding] = settings.value(ctx + "/" + keybinding).toString();
    json[ctx] = ctxJson;
  }
  return json;
}

void PreferencesDialog::groupFromJson(QSettings& settings, QJsonObject obj) {
  QStringList ctxGroups = obj.keys();
  for (QString ctx : ctxGroups) {
    settings.beginGroup(ctx);
    QJsonObject ctxJson = obj[ctx].toObject();
    QStringList keybindings = ctxJson.keys();
    for (QString keybinding : keybindings)
      settings.setValue(keybinding, ctxJson[keybinding].toString());
    settings.endGroup();
  }
}

void PreferencesDialog::setupKeybindingUI() {
  ui->keybindingTree->clear();
  ui->keybindingTree->header()->setSectionResizeMode(0,QHeaderView::Stretch);
  ui->keybindingTree->header()->setSectionResizeMode(1,QHeaderView::Stretch);

  foreach (KeybindingContext kctx, keybindingContexts) {
    QTreeWidgetItem *item = new QTreeWidgetItem({kctx.first,""});

    QFont boldFont = item->font(0);
    boldFont.setBold(true);
    item->setFont(0,boldFont);

    ui->keybindingTree->addTopLevelItem(item);
  }

  connect(ui->keybindingButtons->button(QDialogButtonBox::RestoreDefaults), &QAbstractButton::clicked, this,
          [&]() {
    QSettings settings;
    settings.beginGroup(preferencesKey());
    settings.remove(keybindingKey());
    settings.endGroup(); // Preferences/Keybinding
    this->setupKeybindingContextUI();
  });

  connect(ui->keybindingButtons->button(QDialogButtonBox::Open), &QAbstractButton::clicked, this,
          []() {
    QString openFileName = QFileDialog::getOpenFileName(
          nullptr, tr("Open Keybindings"), "",
          tr("All Files (*.*);;JSON (*.json)")
    );
    QFile openFile(openFileName);

    openFile.open(QFile::ReadOnly | QFile::Text);
    QJsonDocument doc = QJsonDocument::fromJson(openFile.readAll());
    openFile.close();

    QSettings settings;
    settings.beginGroup(preferencesKey());
    settings.beginGroup(keybindingKey());

    groupFromJson(settings, doc.object());

    settings.endGroup();
    settings.endGroup();
  });

  connect(ui->keybindingButtons->button(QDialogButtonBox::Save), &QAbstractButton::clicked, this,
          []() {
    QString saveFileName = QFileDialog::getSaveFileName(
          nullptr, tr("Save Keybindings"), "",
          tr("All Files (*.*);;JSON (*.json)")
    );
    QFile saveFile(saveFileName);

    QSettings settings;
    settings.beginGroup(preferencesKey());
    settings.beginGroup(keybindingKey());

    QJsonObject json = groupToJson(settings);

    settings.endGroup();
    settings.endGroup();

    QJsonDocument doc(json);
    saveFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    saveFile.write(doc.toJson());
    saveFile.close();
  });
}

void PreferencesDialog::setupKeybindingContextUI() {
  R_EXPECT_V(keybindingContexts.size() == ui->keybindingTree->topLevelItemCount());
  for (int i = 0; i < keybindingContexts.size(); ++i) {
    auto tctx = ui->keybindingTree->topLevelItem(i);
    while (tctx->childCount() > 0)
      tctx->removeChild(tctx->child(0));

    auto kctx = keybindingContexts[i];
    std::function<QWidget*()> ctxFactory = kctx.second;
    QWidget *widget = ctxFactory();
    R_EXPECT_V(widget);

    QList<QAction*> actions = widget->findChildren<QAction*>(QString(),Qt::FindDirectChildrenOnly);
    for (int i = 0; i < actions.size(); ++i) {
      auto action = actions[i];
      auto item = new QTreeWidgetItem();

      QIcon icon = action->icon();
      if (icon.isNull()) {
        QPixmap pm(18,18);
        pm.fill(Qt::transparent);
        icon = QIcon(pm);
      }
      item->setIcon(0, icon);

      auto shortcutEdit = new QKeySequenceEdit(action->shortcut());
      QLabel *label = new QLabel(action->text());
      label->setBuddy(label);

      tctx->addChild(item);
      ui->keybindingTree->setItemWidget(item,0,label);
      ui->keybindingTree->setItemWidget(item,1,shortcutEdit);
    }

    tctx->setExpanded(true);
    delete widget;
  }
}

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
    if (style()->objectName().toLower() == styleName.toLower())
      ui->styleCombo->setCurrentText(styleName);
  }

  this->setupKeybindingContextUI();
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
