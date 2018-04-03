#ifndef SPRITEEDITOR_H
#define SPRITEEDITOR_H

#include "BaseEditor.h"

namespace Ui {
class SpriteEditor;
}

class SpriteEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit SpriteEditor(ProtoModel* model, QWidget* parent);
  ~SpriteEditor();

 private:
  Ui::SpriteEditor* ui;
};

#endif  // SPRITEEDITOR_H
