#include "DragListView.h"

#include <QDebug>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

DragListView::DragListView(QWidget *parent) : QListView(parent) {}
