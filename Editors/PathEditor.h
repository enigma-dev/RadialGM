#ifndef PATHEDITOR_H
#define PATHEDITOR_H

#include "BaseEditor.h"
#include "Components/ArtManager.h"

#include <QLabel>

namespace Ui {
class PathEditor;
}

class PathEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit PathEditor(ProtoModelPtr model, QWidget* parent);
  ~PathEditor() override;

 private:
  Ui::PathEditor* ui;
  QLabel* cursorPositionLabel;
};

#endif  // PATHEDITOR_H
