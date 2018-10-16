#include "SettingsEditor.h"
#include "ui_SettingsEditor.h"

SettingsEditor::SettingsEditor(ProtoModel* model, QWidget* parent)
    : BaseEditor(model, parent), ui(new Ui::SettingsEditor) {
  ui->setupUi(this);
}

SettingsEditor::~SettingsEditor() { delete ui; }
