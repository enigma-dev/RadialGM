#ifndef BACKGROUNDEDITOR_H
#define BACKGROUNDEDITOR_H

#include "BaseEdtior.h"

#include "Widgets/BackgroundRenderer.h"

namespace Ui {
class BackgroundEditor;
}

class BackgroundEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit BackgroundEditor(QWidget *parent, ProtoModel *model);
  ~BackgroundEditor();

 private slots:
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                   const QVector<int> &roles = QVector<int>()) override;
  void on_actionSave_triggered();
  void on_actionZoomIn_triggered();
  void on_actionZoomOut_triggered();
  void on_actionZoom_triggered();
  void on_actionNewImage_triggered();
  void on_actionLoadImage_triggered();
  void on_actionSaveImage_triggered();
  void on_actionEditImage_triggered();

 private:
  Ui::BackgroundEditor *ui;
  BackgroundRenderer *renderer;
};

#endif  // BACKGROUNDEDITOR_H
