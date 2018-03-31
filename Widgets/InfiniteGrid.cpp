#include "InfiniteGrid.h"

#include "Components/Utility.h"

#include <QGuiApplication>
#include <QKeyEvent>
#include <QPainter>

#include <QDebug>

#include <cstdlib>

InfiniteGrid::InfiniteGrid(QWidget *parent) : QWidget(parent), xPos(0), yPos(0) { setMouseTracking(true); }

void InfiniteGrid::keyReleaseEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Control) setCursor(QCursor(Qt::ArrowCursor));
  event->accept();
}
void InfiniteGrid::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Control) setCursor(QCursor(Qt::SizeAllCursor));
  if (event->key() == Qt::Key_Right) {
    xPos += 8;
    update();
  }
  if (event->key() == Qt::Key_Left) {
    xPos -= 8;
    update();
  }
  if (event->key() == Qt::Key_Up) {
    yPos -= 8;
    update();
  }
  if (event->key() == Qt::Key_Down) {
    yPos += 8;
    update();
  }
  event->accept();
}

void InfiniteGrid::mouseMoveEvent(QMouseEvent * /*event*/) { update(); }

void InfiniteGrid::update() {
  QPoint mappedPoint = mapFromGlobal(QCursor::pos());
  QPoint cursor = QPoint(RoundNearest<int>(mappedPoint.x() - size().width() / 2 - xPos, 64),
                         RoundNearest<int>(mappedPoint.y() - size().height() / 2 - yPos, 64));

  emit MouseMoved(cursor.x(), cursor.y());
  QWidget::update();
}

QPoint InfiniteGrid::MapToGrid(const QPoint &pos) {
  QPoint cursor = mapFromGlobal(QCursor::pos());
  int halfWidth = size().width() / 2;
  //int relXPos = xPos % halfWidth;
  int x = std::round((cursor.x() - xPos - halfWidth) / (float)64) * 64 + xPos + halfWidth;

  return QPoint(x, 0);
  ;
}

void InfiniteGrid::paintEvent(QPaintEvent * /* event */) {
  QPainter painter(this);
  painter.fillRect(QRect(QPoint(0, 0), size()), QBrush(Qt::black));

  //painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
  painter.setPen(QColor(0xff, 0xff, 0xff, 100));

  int gridWidth = 64;
  int gridHeight = 64;

  int halfWidth = size().width() / 2;
  int relXPos = xPos % halfWidth;
  for (int x = halfWidth + gridWidth + relXPos; x < size().width(); x += gridWidth) {
    painter.drawLine(x, 0, x, size().height());
  }

  for (int x = halfWidth - gridWidth + relXPos; x > 0; x -= gridWidth) {
    painter.drawLine(x, 0, x, size().height());
  }

  int halfHeight = size().height() / 2;
  int relYPos = yPos % halfHeight;
  for (int y = halfHeight + gridHeight + relYPos; y < size().height(); y += gridHeight) {
    painter.drawLine(0, y, size().width(), y);
  }

  for (int y = halfHeight - gridHeight + relYPos; y > 0; y -= gridHeight) {
    painter.drawLine(0, y, size().width(), y);
  }

  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

  if (xPos < halfWidth && xPos > -halfWidth) painter.setPen(Qt::red);
  painter.drawLine(halfWidth + relXPos, 0, halfWidth + relXPos, size().height());

  painter.setPen(QColor(0xff, 0xff, 0xff, 100));

  if (yPos < halfHeight && yPos > -halfHeight) painter.setPen(Qt::blue);
  painter.drawLine(0, halfHeight + relYPos, size().width(), halfHeight + relYPos);

  painter.setPen(Qt::black);
  QPoint cursor = MapToGrid(QCursor::pos());
  if (cursor.x() < size().width() && cursor.y() < size().height()) {
    painter.setBrush(QBrush(Qt::white));
    painter.drawEllipse(cursor, 6, 6);
  }
}
