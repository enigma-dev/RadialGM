#include "SettingsEditor.h"
#include "MainWindow.h"
#include "ui_SettingsEditor.h"

#include <QDebug>

SettingsEditor::SettingsEditor(ProtoModel* model, QWidget* parent)
    : BaseEditor(model, parent), ui(new Ui::SettingsEditor) {
  ui->setupUi(this);

  const QMap<QString, QComboBox*> systemUIMap = {
      {QString("Audio"), ui->audioCombo},          {QString("Platform"), ui->platformCombo},
      {QString("Graphics"), ui->graphicsCombo},    {QString("Widget"), ui->widgetsCombo},
      {QString("Collision"), ui->collisionsCombo}, {QString("Compilers"), ui->compilerCombo},
      {QString("Network"), ui->networkingCombo},
  };
  foreach (auto system, MainWindow::systemCache) {
    const QString systemName = QString::fromStdString(system.name());
    auto it = systemUIMap.find(systemName);
    if (it == systemUIMap.end()) continue;
    foreach (auto subsystem, system.subsystems()) {
      const QString subsystemName = QString::fromStdString(subsystem.name());
      auto combo = it.value();
      combo->addItem(subsystemName);
    }
  }
}

SettingsEditor::~SettingsEditor() { delete ui; }
