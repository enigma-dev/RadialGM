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

int PathView::Size() const {
  RepeatedProtoModelPtr pointsModel = pathModel->GetRepeatedSubModel(Path::kPointsFieldNumber);
  return pointsModel->rowCount();
}

bool PathView::Closed() const { return pathModel->data(Path::kClosedFieldNumber).toBool(); }
bool PathView::Smooth() const { return pathModel->data(Path::kSmoothFieldNumber).toBool(); }
int PathView::Precision() const {
  QVariant prec = pathModel->data(Path::kPrecisionFieldNumber);
  if (!prec.isValid()) return 4;
  int res = prec.toInt();
  if (res < 1) return 1;
  if (res > 8) return 8;
  return res;
}

int PathView::EffectiveIndex(int n, int size, bool closed) const {
  if (closed) {
    if (n < 0) {
      if (n < -size) n %= size;
      n += size;
    }
    return n % size;
  } else {
    if (n <= 0) return 0;
    const int max = Size() - 1;
    if (n >= max) return max;
    return n;
  }
}
QPoint PathView::EffectivePoint(int n, int size, bool closed) const { return Point(EffectiveIndex(n, size, closed)); }
QPoint PathView::Point(int n) const {
  RepeatedProtoModelPtr pointsModel = pathModel->GetRepeatedSubModel(Path::kPointsFieldNumber);
  return QPoint(pointsModel->data(n, Path::Point::kXFieldNumber).toInt(),
                pointsModel->data(n, Path::Point::kYFieldNumber).toInt());
}

namespace {

// Draws a curve of adjustable shittiness. Use increment to control the
// shittiness. For added shittiness, the start and end points will not be drawn.
void ApproximateCurve(QPoint start, QPoint handle, QPoint end, double increment, QVector<QPoint> *points) {
  for (double i = increment; i < 1; i += increment) {
    points->push_back(quadratic(start, handle, end, i));
  }
}

// A curve function that doesn't do what it's fucking told, and instead goes
// off and does its own thing, because apparently Mark doesn't know what a
// damned spline is
void OvermarsCurve(QPoint previous, QPoint current, QPoint next, double increment, QVector<QPoint> *points) {
  QPoint a = (previous + current) / 2;
  QPoint b = current;
  QPoint c = (current + next) / 2;

  ApproximateCurve(a, b, c, increment, points);
}

}  // namespace

QVector<QPoint> PathView::RenderPoints(const QVector<QPoint> &user_points) const {
  QVector<QPoint> points;
  if (user_points.empty()) return points;

  const bool closed = Closed();
  int orig_precision = Precision();
  double increment = pow(2, -orig_precision);

  if (Smooth()) {
    // We'll be digesting a sliding window of three points at a time.
    if (user_points.size() < 2) return points;
    QPoint p1;
    QPoint p2 = user_points[0];
    QPoint p3 = user_points[1];
    // Only add the first point itself if the path is open.
    // Otherwise, we won't be visiting any of the points the user specified,
    // because that would be an intelligent thing to do.
    if (!closed) points.push_back(p2);
    for (int i = 0; i < user_points.size() - 2; ++i) {
      p1 = p2;
      p2 = p3;
      p3 = user_points[i + 2];
      OvermarsCurve(p1, p2, p3, increment, &points);
    }
    if (closed) {
      p1 = p2;
      p2 = p3;
      p3 = user_points[0];
      QPoint p4 = user_points[1];
      OvermarsCurve(p1, p2, p3, increment, &points);
      OvermarsCurve(p2, p3, p4, increment, &points);
      points.push_back(points[0]);
    } else {
      points.push_back(user_points.back());
    }
  } else {
    for (const QPoint &point : user_points) points.push_back(point);
    if (closed) points.push_back(user_points[0]);
  }
  return points;
}

void PathView::Paint(QPainter &painter) {
  RoomView::Paint(painter);

  RepeatedProtoModelPtr pointsModel = pathModel->GetRepeatedSubModel(Path::kPointsFieldNumber);
  if (!pointsModel->empty()) {
    int size = pointsModel->rowCount();

    QPainterPath path;
    QVector<QPoint> user_points(size);
    for (int i = 0; i < size; ++i) user_points[i] = Point(i);

    QVector<QPoint> rendered_points = RenderPoints(user_points);
    path.moveTo(rendered_points[0]);
    for (int i = 0; i < rendered_points.size(); ++i) {
      path.lineTo(rendered_points[i]);
    }

    painter.setPen(QPen(Qt::white, 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);

    painter.setPen(QPen(Qt::black, 1));

    painter.setBrush(QBrush(Qt::blue));
    for (const QPoint &point : user_points) {
      painter.drawEllipse(point, 4, 4);
    }

    painter.setBrush(QBrush(Qt::green));
    if (rendered_points.size() >= 2) {
      QLineF start_vector(rendered_points[0], rendered_points[1]);
      float angle = start_vector.angle();

      painter.save();
      painter.translate(rendered_points[0]);
      painter.rotate(90 - angle);
      const QPolygon startArrow({{0, -8}, {-6, 8}, {0, 4}, {6, 8}});
      painter.drawPolygon(startArrow);
      painter.restore();
    }

    if (selectedPointIndex != -1 && selectedPointIndex < user_points.size()) {
      painter.setBrush(QBrush(Qt::red));
      painter.drawEllipse(user_points[selectedPointIndex], 4, 4);
    }
  }

  painter.setBrush(QBrush(Qt::yellow));
  painter.drawEllipse(mousePos, 4, 4);
}

void PathView::SetPathModel(ProtoModelPtr model) { pathModel = model; }
