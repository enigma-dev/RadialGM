#ifndef ROOMEDITOR_H
#define ROOMEDITOR_H

#include "BaseEditor.h"
#include "Components/ArtManager.h"

#include <QGraphicsScene>
#include <QPainter>

namespace Ui {
class RoomEditor;
}

class RoomEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit RoomEditor(ProtoModel* model, QWidget* parent);
  ~RoomEditor();

  void setZoom(qreal zoom);

 private slots:
  void on_actionZoomIn_triggered();
  void on_actionZoomOut_triggered();
  void on_actionZoom_triggered();

 private:
  Ui::RoomEditor* ui;
  qreal zoom = 1.0;

  bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif  // ROOMEDITOR_H
