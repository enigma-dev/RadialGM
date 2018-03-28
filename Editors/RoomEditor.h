#ifndef ROOMEDITOR_H
#define ROOMEDITOR_H

#include "BaseEditor.h"

namespace Ui {
class RoomEditor;
}

class RoomEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit RoomEditor(ProtoModel* model, QWidget* parent);
  ~RoomEditor();

 private:
  Ui::RoomEditor* ui;
};

#endif  // ROOMEDITOR_H
