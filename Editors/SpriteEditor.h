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
  explicit SpriteEditor(ProtoModelPtr model, QWidget* parent);
  ~SpriteEditor() override;
  void LoadedMismatchedImage(QSize expectedSize, QSize actualSize);

 public slots:
  void RebindSubModels() override;

 private:
  Ui::SpriteEditor* ui;
  SpriteModel* spriteModel;
};

#endif  // SPRITEEDITOR_H
