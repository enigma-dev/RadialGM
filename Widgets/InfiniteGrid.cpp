#include "InfiniteGrid.h"

#include "Components/Utility.h"

#include <QGuiApplication>
#include <QKeyEvent>
#include <QPainter>

#include <QDebug>

#include <cstdlib>

InfiniteGrid::InfiniteGrid(QWidget *parent) : QWidget(parent) { setMouseTracking(true); }

void InfiniteGrid::SetPixmap(QPixmap pixmap) { this->pixmap = pixmap; }

void InfiniteGrid::keyReleaseEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Right || event->key() == Qt::Key_Right) hSpeed = 0;
  if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) vSpeed = 0;

  event->accept();
}

void InfiniteGrid::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Right) hSpeed = gridScrollSpeed;
  if (event->key() == Qt::Key_Left) hSpeed = -gridScrollSpeed;
  if (event->key() == Qt::Key_Up) vSpeed = -gridScrollSpeed;
  if (event->key() == Qt::Key_Down) vSpeed = gridScrollSpeed;

  event->accept();
  update();
}

void InfiniteGrid::mouseMoveEvent(QMouseEvent * /*event*/) { update(); }

void InfiniteGrid::update() {
  xPos += hSpeed;
  yPos += vSpeed;

  QPoint mappedPoint = mapFromGlobal(QCursor::pos());
  QPoint cursor = QPoint(RoundNearest<int>(mappedPoint.x() - size().width() / 2 - xPos, 64),
                         RoundNearest<int>(mappedPoint.y() - size().height() / 2 - yPos, 64));

  emit MouseMoved(cursor.x(), cursor.y());
  QWidget::update();
}

QPoint InfiniteGrid::MapToGrid(const QPoint &pos) {
  QPoint cursor = mapFromGlobal(pos);
  const int halfWidth = size().width() / 2;
  const int halfHeight = size().height() / 2;
  const int gridWidth = 64;
  const int gridHeight = 64;

  int x = static_cast<int>(std::round((cursor.x() - xPos - halfWidth) / static_cast<float>(gridWidth)) * gridWidth +
                           xPos + halfWidth);
  int y = static_cast<int>(std::round((cursor.y() - yPos - halfHeight) / static_cast<float>(gridHeight)) * gridHeight +
                           yPos + halfHeight);

  return QPoint(x, y);
  ;
}

void InfiniteGrid::paintEvent(QPaintEvent * /* event */) {
  QPainter painter(this);
  painter.fillRect(QRect(QPoint(0, 0), size()), QBrush(Qt::black));

  if (resetView) {
    xPos = -qMin(pixmap.size().width() / 2, size().width() / 2);
    yPos = -qMin(pixmap.size().height() / 2, size().height() / 2);
    resetView = false;
  }

  painter.drawPixmap(QPoint(size().width() / 2 + xPos, size().height() / 2 + yPos), pixmap);

  //painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
  painter.setPen(QColor(0xff, 0xff, 0xff, 100));

  int gridWidth = 64;
  int gridHeight = 64;

  int halfWidth = size().width() / 2;
  int relXPos = xPos % gridWidth;
  for (int x = halfWidth + relXPos; x < size().width(); x += gridWidth) {
    painter.drawLine(x, 0, x, size().height());
  }

  for (int x = halfWidth - gridWidth + relXPos; x > 0; x -= gridWidth) {
    painter.drawLine(x, 0, x, size().height());
  }

  int halfHeight = size().height() / 2;
  int relYPos = yPos % gridHeight;
  for (int y = halfHeight + relYPos; y < size().height(); y += gridHeight) {
    painter.drawLine(0, y, size().width(), y);
  }

  for (int y = halfHeight - gridHeight + relYPos; y > 0; y -= gridHeight) {
    painter.drawLine(0, y, size().width(), y);
  }

  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

  if (xPos < halfWidth && xPos > -halfWidth) painter.setPen(Qt::red);
  painter.drawLine(halfWidth + xPos, 0, halfWidth + xPos, size().height());

  painter.setPen(QColor(0xff, 0xff, 0xff, 100));

  if (yPos < halfHeight && yPos > -halfHeight) painter.setPen(Qt::blue);
  painter.drawLine(0, halfHeight + yPos, size().width(), halfHeight + yPos);

  painter.setPen(Qt::black);
  QPoint cursor = MapToGrid(QCursor::pos());
  if (cursor.x() < size().width() && cursor.y() < size().height()) {
    painter.setBrush(QBrush(Qt::white));
    painter.drawEllipse(cursor, 6, 6);
  }
}
