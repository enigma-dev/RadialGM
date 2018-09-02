#ifndef SPRITEEDITOR_H
#define SPRITEEDITOR_H

#include "BaseEditor.h"

#include "Models/SpriteModel.h"

namespace Ui {
class SpriteEditor;
}

class SpriteEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit SpriteEditor(ProtoModel* model, QWidget* parent);
  ~SpriteEditor();
  void LoadedMismatedImage(QSize expectedSize, QSize actualSize);

 private:
  Ui::SpriteEditor *ui;
  SpriteModel *spriteModel;
};

#endif  // SPRITEEDITOR_H
