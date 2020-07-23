#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include "PreferencesKeys.h"
#include "main.h"
#include "Components/Logger.h"

#include "ui_MainWindow.h"
#include "ui_SpriteEditor.h"
#include "ui_SoundEditor.h"
#include "ui_BackgroundEditor.h"
#include "ui_PathEditor.h"
#include "ui_FontEditor.h"
#include "ui_SettingsEditor.h"
#include "ui_TimelineEditor.h"
#include "ui_ObjectEditor.h"
#include "ui_RoomEditor.h"

#include <QKeySequenceEdit>
#include <QPushButton>
#include <QSettings>
#include <QStyleFactory>

template <typename T, typename P = QWidget>
P *keybindingFactory() {
  P* context = new P();
  T *form = new T();
  form->setupUi(context);
  return context;
}

using KeybindingFactory = std::function<QWidget*()>;
using KeybindingContext = QPair<QString,KeybindingFactory>;
static QList<KeybindingContext> keybindingContexts = {
  {QObject::tr("Global"),keybindingFactory<Ui::MainWindow,QMainWindow>},
  {QObject::tr("Sprite Editor"),keybindingFactory<Ui::SpriteEditor>},
  {QObject::tr("Sound Editor"),keybindingFactory<Ui::SoundEditor>},
  {QObject::tr("Background Editor"),keybindingFactory<Ui::BackgroundEditor>},
  {QObject::tr("Path Editor"),keybindingFactory<Ui::PathEditor>},
  //{QObject::tr("Script Editor"),keybindingFactory<Ui::ScriptEditor>},
  //{QObject::tr("Shader Editor"),keybindingFactory<Ui::ShaderEditor>},
  {QObject::tr("Font Editor"),keybindingFactory<Ui::FontEditor>},
  {QObject::tr("Timeline Editor"),keybindingFactory<Ui::TimelineEditor>},
  {QObject::tr("Object Editor"),keybindingFactory<Ui::ObjectEditor>},
  {QObject::tr("Room Editor"),keybindingFactory<Ui::RoomEditor>}
};

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PreferencesDialog) {
  ui->setupUi(this);
  this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

  foreach (QString styleName, QStyleFactory::keys()) { ui->styleCombo->addItem(styleName); }
  this->setupKeybindingUI();
  this->setupKeybindingContextUI();
  this->reset();

  connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QAbstractButton::clicked, this,
          &PreferencesDialog::restoreDefaultsClicked);
  connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this,
          &PreferencesDialog::applyClicked);
}

PreferencesDialog::~PreferencesDialog() { delete ui; }

void PreferencesDialog::setupKeybindingUI() {
  ui->keybindingTree->clear();
  // ui->keybindingTree->headerItem()->setFirstColumnSpanned(true);
  ui->keybindingTree->header()->resizeSections(QHeaderView::ResizeToContents);
  ui->keybindingTree->header()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
  ui->keybindingTree->header()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
  ui->keybindingTree->header()->setSectionResizeMode(2,QHeaderView::Stretch);

  foreach (KeybindingContext kctx, keybindingContexts) {
    QTreeWidgetItem *item = new QTreeWidgetItem({kctx.first,"",""});
    //item->setFirstColumnSpanned(true);

    QFont boldFont = item->font(0);
    boldFont.setBold(true);
    item->setFont(0,boldFont);
    ui->keybindingTree->addTopLevelItem(item);
  }
}

void PreferencesDialog::setupKeybindingContextUI() {
  R_EXPECT_V(keybindingContexts.size() == ui->keybindingTree->topLevelItemCount());
  for (int i = 0; i < keybindingContexts.size(); ++i) {
    auto tctx = ui->keybindingTree->topLevelItem(i);
    for (int j = 0; j < tctx->childCount(); ++j)
      tctx->removeChild(tctx->child(j));

    auto kctx = keybindingContexts[i];
    std::function<QWidget*()> ctxFactory = kctx.second;
    QWidget *widget = ctxFactory();
    R_EXPECT_V(widget);

    QList<QAction*> actions = widget->findChildren<QAction*>(QString(),Qt::FindDirectChildrenOnly);
    for (int i = 0; i < actions.size(); ++i) {
      auto action = actions[i];
      auto item = new QTreeWidgetItem();
      //item->setFirstColumnSpanned(true);
      item->setIcon(1, action->icon());

      auto shortcutEdit = new QKeySequenceEdit(action->shortcut());
      QLabel *label = new QLabel(action->text());
      label->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
      label->setBuddy(shortcutEdit);

      tctx->addChild(item);
      ui->keybindingTree->setItemWidget(item,0,label);
      ui->keybindingTree->setItemWidget(item,2,shortcutEdit);
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
