#include "PathView.h"

PathView::PathView(AssetScrollAreaBackground *parent) : RoomView(parent) {}

// Perform cubic bezier interpolation
QPoint cubic(QPoint start, QPoint handle1, QPoint handle2, QPoint end, double position) {
  double inv_position = 1 - position;
  QPoint a3 = start * inv_position + handle1 * position;
  QPoint b3 = handle1 * inv_position + handle2 * position;
  QPoint c3 = handle2 * inv_position + end * position;
  QPoint a2 = a3 * inv_position + b3 * position;
  QPoint b2 = b3 * inv_position + c3 * position;
  return a2 * inv_position + b2 * position;
}
// Perform quadratic bezier interpolation
QPoint quadratic(QPoint start, QPoint handle, QPoint end, double position) {
  double inv_position = 1 - position;
  QPoint a = start * inv_position + handle * position;
  QPoint b = handle * inv_position + end * position;
  return a * inv_position + b * position;
}

void shittyCurve(QPoint start, QPoint handle, QPoint end, QPainterPath &path) {
  int precision = 4;
    path.moveTo(start);
  for (int i = 1; i <= precision; ++i) {
    double at = i / (double)precision;
    path.lineTo(quadratic(start, handle, end, at));
  }
}

void whatever(QPoint previous, QPoint current, QPoint next, int precision, QPainterPath &path) {
  QPoint a = (previous + current) / 2;
  QPoint b = current;
  QPoint c = (current + next) / 2;
  
  shittyCurve(a, b, c, path);
  path.addEllipse(a, 9, 5);
  path.addEllipse(b, 9, 9);
  path.addEllipse(c, 5, 9);
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

  QPoint prev = closed ? end : start;
  QPoint cap = closed ? start : end;
  for (int p = 0; p < pointsModel->rowCount(); ++p) {
    QPoint cur(pointsModel->data(p, Path::Point::kXFieldNumber).toInt(),
               pointsModel->data(p, Path::Point::kYFieldNumber).toInt());
    QPoint next = p + 1 < pointsModel->rowCount()
         ? QPoint(pointsModel->data(p + 1, Path::Point::kXFieldNumber).toInt(),
                  pointsModel->data(p + 1, Path::Point::kYFieldNumber).toInt())
         : cap;

    QVariant prec = pathModel->data(Path::kPrecisionFieldNumber);
    int precision = prec.isValid() ? prec.toInt() : 4;

    whatever(prev, cur, next, precision, path);
    path.addEllipse(cur, 4, 4);
    prev = cur;
  }

  painter.drawPath(path);

  painter.setBrush(QBrush(Qt::yellow));
  painter.drawEllipse(mousePos, 4, 4);
}

void PathView::SetPathModel(ProtoModelPtr model) { pathModel = model; }
