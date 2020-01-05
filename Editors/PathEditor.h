#ifndef PATHEDITOR_H
#define PATHEDITOR_H

#include "BaseEditor.h"
#include "Components/ArtManager.h"

#include <QLabel>
#include <QLineEdit>

namespace Ui {
class PathEditor;
}

class PathEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit PathEditor(ProtoModelPtr model, QWidget* parent);
  ~PathEditor() override;
  void RebindSubModels() override;
  bool eventFilter(QObject* obj, QEvent* event) override;

 private:
  Ui::PathEditor* ui;
  QLabel* cursorPositionLabel;
  QLineEdit* roomLineEdit;
  bool snapToGrid = true;
};

#endif  // PATHEDITOR_H
