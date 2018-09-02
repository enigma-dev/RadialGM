#ifndef SOUNDEDITOR_H
#define SOUNDEDITOR_H

#include "BaseEditor.h"

namespace Ui {
class SoundEditor;
}

class SoundEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit SoundEditor(ProtoModel* model, QWidget* parent);
  ~SoundEditor();

 private:
  Ui::SoundEditor* ui;
};

#endif  // SOUNDEDITOR_H
