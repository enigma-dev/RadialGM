#ifndef FONTEDITOR_H
#define FONTEDITOR_H

#include "BaseEditor.h"

namespace Ui {
class FontEditor;
}

class FontEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit FontEditor(ProtoModelPtr model, QWidget* parent);
  ~FontEditor();

 private:
  Ui::FontEditor* ui;
};

#endif  // FONTEDITOR_H
