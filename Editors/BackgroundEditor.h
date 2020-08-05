#ifndef BACKGROUNDEDITOR_H
#define BACKGROUNDEDITOR_H

#include "BaseEditor.h"

#include "Widgets/BackgroundView.h"

namespace Ui {
class BackgroundEditor;
}

class BackgroundEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit BackgroundEditor(EditorModel* model, QWidget* parent);
  ~BackgroundEditor() override;

 private slots:
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVariant &oldValue = QVariant(0),
                   const QVector<int> &roles = QVector<int>()) override;
  void on_actionZoomIn_triggered();
  void on_actionZoomOut_triggered();
  void on_actionZoom_triggered();
  void on_actionNewImage_triggered();
  void on_actionLoadImage_triggered();
  void on_actionSaveImage_triggered();
  void on_actionEditImage_triggered();

 private:
  void RebindSubModels() override;
  Ui::BackgroundEditor *_ui;
  ProtoModel *_backgroundModel;
  BackgroundView *_renderer;
};

#endif  // BACKGROUNDEDITOR_H
