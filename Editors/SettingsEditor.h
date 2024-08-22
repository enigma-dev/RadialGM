#ifndef SETTINGSEDITOR_H
#define SETTINGSEDITOR_H

#include "BaseEditor.h"

#include <QListWidgetItem>

namespace Ui {
class SettingsEditor;
}

class SettingsEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit SettingsEditor(MessageModel* model, QWidget* parent);
  void RebindSubModels() override;
  ~SettingsEditor();

 private slots:
  void on_listWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);

 private:
  Ui::SettingsEditor* ui;
  QMap<QString, QWidget*> pageMap;
};

#endif  // SETTINGSEDITOR_H
