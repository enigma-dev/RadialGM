#ifndef SPRITEEDITOR_H
#define SPRITEEDITOR_H

#include "BaseEdtior.h"

namespace Ui {
class SpriteEditor;
}

class SpriteEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit SpriteEditor(QWidget *parent, ProtoModel *model);
  ~SpriteEditor();

 private:
  Ui::SpriteEditor *ui;
};

#endif  // SPRITEEDITOR_H
