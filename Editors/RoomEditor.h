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
  explicit RoomEditor(EditorModel* model, QWidget* parent);
  ~RoomEditor() override;

  void setZoom(qreal zoom);

 public slots:
  void SelectedObjectChanged(QAction* action);

 private slots:
  void on_actionZoomIn_triggered();
  void on_actionZoomOut_triggered();
  void on_actionZoom_triggered();
  void updateCursorPositionLabel(const QPoint& pos);

 private:
  Ui::RoomEditor* _ui;
  QLabel *cursorPositionLabel, *_assetNameLabel;
  ImmediateDataWidgetMapper* _viewMapper;
};

#endif  // ROOMEDITOR_H
