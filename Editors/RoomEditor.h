#ifndef ROOMEDITOR_H
#define ROOMEDITOR_H

#include "BaseEditor.h"
#include "Components/ArtManager.h"

#include <QGraphicsScene>
#include <QLabel>
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

 public slots:
  void SelectedObjectChanged(QAction *action);

 private slots:
  void on_actionZoomIn_triggered();
  void on_actionZoomOut_triggered();
  void on_actionZoom_triggered();
  void updateCursorPositionLabel(const QPoint& pos);
  void on_actionSave_triggered();

private:
  Ui::RoomEditor* ui;
  QLabel *cursorPositionLabel, *assetNameLabel;

  bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif  // ROOMEDITOR_H
