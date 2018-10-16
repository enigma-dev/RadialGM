#ifndef SETTINGSEDITOR_H
#define SETTINGSEDITOR_H

#include "BaseEditor.h"

#include <QTreeWidgetItem>

namespace Ui {
class SettingsEditor;
}

class SettingsEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit SettingsEditor(ProtoModel* model, QWidget* parent);
  ~SettingsEditor();

 private slots:
  void on_treeWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

 private:
  Ui::SettingsEditor* ui;
  QMap<QString, QWidget*> treePageMap;
};

#endif  // SETTINGSEDITOR_H
