#include "SpriteSubimageListView.h"

#include <QDebug>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>

SpriteSubimageListView::SpriteSubimageListView(QWidget* parent) : QListView(parent) {}

int SpriteSubimageListView::GetDropIndex(QDropEvent* e) {
  QPoint t(e->pos().x() / gridSize().width(), e->pos().y() / gridSize().height());
  return t.y() * (width() / gridSize().width()) + t.x();
}

// Note: Qt's drag & drop behavior is completly fuxed in icon mode so we reimplement our own behavior here
void SpriteSubimageListView::dragMoveEvent(QDragMoveEvent* e) {
  // If the drop is greater than the last grid cell *and* it's an internal move we mark it as an invalid drop
  // However, if it's a drag between *different* widgets we will accept a drop anywhere and append it to the end
  if (GetDropIndex(e) > model()->rowCount() && e->source() == this)
    e->ignore();
  else
    e->accept();
}

void SpriteSubimageListView::dropEvent(QDropEvent* e) {
  int index = GetDropIndex(e);

  bool successfulDrop = false;

  // Qt handles internal move incorrectly when dropping at the end of the model
  // For whatever reason dropmimedata is never called in this case so we are forced to call it manually
  if (e->source() == this) {             // If an internal move
    if (index == model()->rowCount()) {  // if dropped at end of model
      e->ignore();
      successfulDrop =
          model()->dropMimeData(e->mimeData(), Qt::DropAction::MoveAction, model()->rowCount(), 0, QModelIndex());
    } else {
      // NOTE: For all other *internal* moves, Qt's behavior is correct here (if the model inherits QAbstractListView)
      // However, due to our current ProtoModel inheritence hirearchy we're forced to inherit from QAbstractItemView
      // so again we force our desired behavior here.
      e->ignore();
      successfulDrop = model()->dropMimeData(e->mimeData(), Qt::DropAction::MoveAction, index, 0, QModelIndex());
      //QListView::dropEvent(e); return;
    }
  } else {  // Drop from another widget
    e->ignore();
    if (index > model()->rowCount()) index = model()->rowCount();  // Drop at end if out of range
    successfulDrop = model()->dropMimeData(e->mimeData(), Qt::DropAction::MoveAction, index, 0, QModelIndex());
  }

  // Remove old indexes if needed
  // Don't delete if drop failed
  // Control is copy modifier so don't delete if pressed
  if (successfulDrop && e->keyboardModifiers() != Qt::ControlModifier) {
    QByteArray encodedData = e->mimeData()->data(model()->mimeTypes()[0]);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    SpriteSubimageListView* source = static_cast<SpriteSubimageListView*>(e->source());
    int removeCount = 0;
    while (!stream.atEnd()) {
      QString text;
      stream >> text;
      int delIndex;
      stream >> delIndex;
      source->model()->removeRow(delIndex - removeCount++);
    }
  }
}
