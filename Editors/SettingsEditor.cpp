#include "SettingsEditor.h"
#include "MainWindow.h"
#include "ui_SettingsEditor.h"

#include "codegen/Settings.pb.h"

#include <QPushButton>

using namespace buffers::resources;

SettingsEditor::SettingsEditor(ProtoModel* model, QWidget* parent)
    : BaseEditor(model, parent), ui(new Ui::SettingsEditor) {
  ui->setupUi(this);

  QPushButton* saveButton = ui->buttonBox->button(QDialogButtonBox::Save);
  saveButton->setIcon(QIcon(":/actions/accept.png"));
  QPushButton* discardButton = ui->buttonBox->button(QDialogButtonBox::Discard);
  discardButton->setIcon(QIcon(":/actions/cancel.png"));

  treePageMap = {{QString("ENIGMA/API"), ui->apiPage}, {QString("ENIGMA/Extensions"), ui->extensionsPage}};

  const QMap<QString, QWidget*> systemUIMap = {
      {QString("Audio"), ui->audioCombo},          {QString("Platform"), ui->platformCombo},
      {QString("Graphics"), ui->graphicsCombo},    {QString("Widget"), ui->widgetsCombo},
      {QString("Collision"), ui->collisionsCombo}, {QString("Compilers"), ui->compilerCombo},
      {QString("Network"), ui->networkingCombo},   {QString("Extensions"), ui->extensionsList},
  };
  foreach (auto system, MainWindow::systemCache) {
    const QString systemName = QString::fromStdString(system.name());
    auto it = systemUIMap.find(systemName);
    if (it == systemUIMap.end()) continue;
    auto widget = it.value();
    const QString className = widget->metaObject()->className();
    QListWidget* listWidget = nullptr;
    QComboBox* combo = nullptr;
    if (className == "QListWidget") {
      listWidget = static_cast<QListWidget*>(widget);
    } else if (className == "QComboBox") {
      combo = static_cast<QComboBox*>(widget);
    }
    foreach (auto subsystem, system.subsystems()) {
      const QString subsystemName = QString::fromStdString(subsystem.name());
      const QString subsystemDesc = QString::fromStdString(subsystem.description());
      if (combo)
        combo->addItem(subsystemName);
      else if (listWidget) {
        auto item = new QListWidgetItem(subsystemName, listWidget);
        item->setFlags(item->flags() | Qt::ItemFlag::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        item->setToolTip(subsystemDesc);
      }
    }
  }
}

SettingsEditor::~SettingsEditor() { delete ui; }

void SettingsEditor::on_treeWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* /*previous*/) {
  QWidget* widget = ui->emptyPage;
  auto item = current;
  if (item) {
    QString path = item->text(0);
    while (item->parent()) {
      item = item->parent();
      path.prepend(item->text(0) + "/");
    }
    auto it = treePageMap.find(path);
    if (it != treePageMap.end()) {
      widget = it.value();
    }
  }

  ui->stackedWidget->setCurrentWidget(widget);
}
