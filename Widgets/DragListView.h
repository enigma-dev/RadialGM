#ifndef DRAGLISTVIEW_H
#define DRAGLISTVIEW_H

#include <QListView>
#include <QWidget>

class DragListView : public QListView {
  Q_OBJECT
 public:
  explicit DragListView(QWidget *parent);
};

#endif  // DRAGLISTVIEW_H
