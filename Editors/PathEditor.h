#ifndef PATHEDITOR_H
#define PATHEDITOR_H

#include "BaseEditor.h"
#include "Components/ArtManager.h"

#include <QItemSelection>
#include <QLabel>
#include <QLineEdit>

namespace Ui {
class PathEditor;
}

class PathEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit PathEditor(ProtoModelPtr model, QWidget* parent);
  ~PathEditor() override;
  void RebindSubModels() override;
  bool eventFilter(QObject* obj, QEvent* event) override;
  void InsertPoint(int index, int x, int y, int speed);

 private slots:
  void SetSnapToGrid(bool snap);
  void RoomMenuItemSelected(QAction* action);
  void RoomMenuButtonPressed();
  void MouseMoved(int x, int y);
  void MousePressed(Qt::MouseButton button);
  void MouseReleased(Qt::MouseButton button);
  void UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected);
  void on_addPointButton_pressed();
  void on_insertPointButton_pressed();
  void on_deletePointButton_pressed();

 private:
  Ui::PathEditor* ui;
  QLabel* cursorPositionLabel;
  QLineEdit* roomLineEdit;
  bool snapToGrid = true;
  ProtoModelPtr pathModel = nullptr;
  RepeatedProtoModelPtr pointsModel = nullptr;
  bool draggingPoint = false;
};

#endif  // PATHEDITOR_H
