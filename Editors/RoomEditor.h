#ifndef ROOMEDITOR_H
#define ROOMEDITOR_H

#include "BaseEdtior.h"

namespace Ui {
class RoomEditor;
}

class RoomEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit RoomEditor(QWidget *parent, ResourceModel *model);
  ~RoomEditor();

 private:
  Ui::RoomEditor *ui;
};

#endif  // ROOMEDITOR_H
