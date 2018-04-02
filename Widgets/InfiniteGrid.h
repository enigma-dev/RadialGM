#ifndef INFINITEGRID_H
#define INFINITEGRID_H

#include <QPixmap>
#include <QWidget>

class InfiniteGrid : public QWidget {
  Q_OBJECT
 public:
  explicit InfiniteGrid(QWidget *parent);
  void SetPixmap(QPixmap pixmap);

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

  QPixmap pixmap;
  bool resetView = true;
  int gridScrollSpeed = 8;
  int hSpeed = 0;
  int vSpeed = 0;
  int xPos = 0;
  int yPos = 0;
  int gridW = 16;
  int gridH = 16;
};

#endif  // INFINITEGRID_H
