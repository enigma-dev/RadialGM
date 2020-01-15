#ifndef SPRITEEDITOR_H
#define SPRITEEDITOR_H

#include "BaseEditor.h"
#include "Models/RepeatedImageModel.h"

#include <QItemSelection>

namespace Ui {
class SpriteEditor;
}

class SpriteEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit SpriteEditor(ProtoModelPtr model, QWidget* parent);
  ~SpriteEditor() override;
  void LoadedMismatchedImage(QSize expectedSize, QSize actualSize);
  void RemoveSelectedIndexes();

 public slots:
  void RebindSubModels() override;
  void SelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

 private slots:
  void on_bboxComboBox_currentIndexChanged(int index);
  void on_actionNewSubimage_triggered();
  void on_actionDeleteSubimages_triggered();
  void on_actionCut_triggered();
  void on_actionPaste_triggered();
  void on_actionCopy_triggered();
  void on_actionLoadSubimages_triggered();
  void on_actionAddSubimages_triggered();
  void on_actionZoom_triggered();
  void on_actionZoomIn_triggered();
  void on_actionZoomOut_triggered();
  void on_actionEditSubimages_triggered();
  void on_centerOriginButton_clicked();

 private:
  Ui::SpriteEditor* ui;
  ProtoModel* _spriteModel;
  RepeatedImageModel* _subimagesModel;
};

#endif  // SPRITEEDITOR_H
