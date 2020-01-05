#include "PathView.h"

PathView::PathView(AssetScrollAreaBackground *parent) : RoomView(parent) {}

// Perform cubic bezier interpolation
QPoint cubic(QPoint start, QPoint handle1, QPoint handle2, QPoint end, double position) {
  double inv_position = 1 - position;
  QPoint a = start * inv_position + handle1 * position;
  QPoint b = handle2 * inv_position + end * position;
  return a * inv_position + b * position;
}

void shittyCurve(QPoint start, QPoint handle1, QPoint handle2, QPoint end, QPainterPath &path) {
  int precision = 4;
  for (int i = 0; i <= precision; ++i) {
    double at = i / (double)precision;
    path.lineTo(cubic(start, handle1, handle2, end, at));
  }
}

void whatever(QPoint previous, QPoint current, QPoint next, QPoint after, int precision, QPainterPath &path) {
  QPoint a = cubic(previous, current, current, next, .5);
  QPoint b = cubic(current, next, next, after, .5);
  shittyCurve(a, next, next, b, path);
}

void PathView::Paint(QPainter &painter) {
  RoomView::Paint(painter);

  RepeatedProtoModelPtr pointsModel = pathModel->GetRepeatedSubModel(Path::kPointsFieldNumber);

  if (pointsModel->empty()) return;

  QPainterPath path;

  //painter.setBrush(QBrush(Qt::blue));

  QPoint start(pointsModel->data(0, Path::Point::kXFieldNumber).toInt(),
               pointsModel->data(0, Path::Point::kYFieldNumber).toInt());

  QPoint end(pointsModel->data(pointsModel->rowCount() - 1, Path::Point::kXFieldNumber).toInt(),
             pointsModel->data(pointsModel->rowCount() - 1, Path::Point::kYFieldNumber).toInt());

  path.moveTo(start);
  painter.setPen(QPen(Qt::white, 3));

  bool closed = pathModel->data(Path::kClosedFieldNumber).toBool();

  QPoint prev = closed ? start : end;
  QPoint cap = closed ? end : start;
  for (int p = 1; p < pointsModel->rowCount() - 1; ++p) {
    QPoint cur(pointsModel->data(p, Path::Point::kXFieldNumber).toInt(),
               pointsModel->data(p, Path::Point::kYFieldNumber).toInt());
    QPoint next(pointsModel->data(p + 1, Path::Point::kXFieldNumber).toInt(),
                pointsModel->data(p + 1, Path::Point::kYFieldNumber).toInt());
    QPoint after = p + 2 < pointsModel->rowCount()
                       ? QPoint(pointsModel->data(p + 2, Path::Point::kXFieldNumber).toInt(),
                                pointsModel->data(p + 2, Path::Point::kYFieldNumber).toInt())
                       : cap;

    QVariant prec = pathModel->data(Path::kPrecisionFieldNumber);
    int precision = prec.isValid() ? prec.toInt() : 4;

    whatever(prev, cur, next, after, precision, path);
    path.addEllipse(next, 4, 4);
    prev = next;
  }

  painter.drawPath(path);

  painter.setBrush(QBrush(Qt::yellow));
  painter.drawEllipse(mousePos, 4, 4);
}

void PathView::SetPathModel(ProtoModelPtr model) { pathModel = model; }
