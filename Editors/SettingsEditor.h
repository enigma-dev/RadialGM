#ifndef SETTINGSEDITOR_H
#define SETTINGSEDITOR_H

#include "BaseEditor.h"

#include "codegen/Settings.pb.h"

namespace Ui {
class SettingsEditor;
}

class SettingsEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit SettingsEditor(ProtoModel* model, QWidget* parent);
  ~SettingsEditor();

 private:
  Ui::SettingsEditor* ui;
};

#endif  // SETTINGSEDITOR_H
