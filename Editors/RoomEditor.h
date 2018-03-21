#ifndef ROOMEDITOR_H
#define ROOMEDITOR_H

#include "Models/ResourceModel.h"

#include <QWidget>

namespace Ui {
class RoomEditor;
}

class RoomEditor : public QWidget {
  Q_OBJECT

 public:
  explicit RoomEditor(QWidget *parent, ResourceModel *model);
  ~RoomEditor();

 private:
  Ui::RoomEditor *ui;
};

#endif  // ROOMEDITOR_H
