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
  explicit RoomEditor(ProtoModelPtr model, QWidget* parent);
  ~RoomEditor() override;

  void setZoom(qreal zoom);

 public slots:
  void RebindSubModels() override;
  void SelectedObjectChanged(QAction* action);

 private slots:
  void on_actionZoomIn_triggered();
  void on_actionZoomOut_triggered();
  void on_actionZoom_triggered();
  void updateCursorPositionLabel(const QPoint& pos);

 private:
  Ui::RoomEditor* ui;
  QLabel *cursorPositionLabel, *assetNameLabel;
  ProtoModelPtr roomModel;
  ImmediateDataWidgetMapper* viewMapper;

  bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif  // ROOMEDITOR_H
