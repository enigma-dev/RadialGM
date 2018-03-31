#ifndef INFINITEGRID_H
#define INFINITEGRID_H

#include <QWidget>

class InfiniteGrid : public QWidget {
  Q_OBJECT
 public:
  explicit InfiniteGrid(QWidget *parent);

 signals:
  void MouseMoved(int x, int y);

 protected slots:
  void update();

 protected:
  QPoint MapToGrid(const QPoint &pos);
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

  int xPos;
  int yPos;
};

#endif  // INFINITEGRID_H
