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
