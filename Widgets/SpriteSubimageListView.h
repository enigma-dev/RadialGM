#ifndef SPRITESUBIMAGELISTVIEW_H
#define SPRITESUBIMAGELISTVIEW_H

#include <QListView>

class SpriteSubimageListView : public QListView {
  Q_OBJECT
 public:
  SpriteSubimageListView(QWidget* parent);

 protected:
  void dragMoveEvent(QDragMoveEvent* e) override;
  void dropEvent(QDropEvent* e) override;
  // Gets the grid cell of potentional drop action
  int GetDropIndex(QDropEvent* e);
  // Clean's up moved items from source mode
  void RemoveMimeIndexes(QMimeData* mimeData, SpriteSubimageListView* source);
  ;
};

#endif  // SPRITESUBIMAGELISTVIEW_H
