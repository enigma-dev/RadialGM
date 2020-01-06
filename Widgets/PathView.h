#ifndef PATHVIEW_H
#define PATHVIEW_H

#include "RoomView.h"

class PathView : public RoomView {
  Q_OBJECT

 public:
  explicit PathView(AssetScrollAreaBackground *parent = nullptr);
  void Paint(QPainter &painter) override;
  void SetPathModel(ProtoModelPtr model);

  // Retrieves the number of points in the path.
  int Size() const;
  // Returns whether the current path is closed.
  bool Closed() const;
  // Returns whether the current path is smooth.
  bool Smooth() const;
  // Returns the precision of the current path, clamped to [1, 8].
  // If unset, returns the default precision, 4.
  // Note that precision is the number of subdivisions, not the number of points
  // used to render each segment, so ocmpute will be exponential in this value.
  int Precision() const;
  // Retrieves the point at the given absolute index. OOB access will cause UB.
  // The given index must be in range [0, Size()).
  QPoint Point(int n) const;
  // Fetches the absolute index for the given effective index, handling OOB
  // values based on whether the path is closed. For open paths, the index is
  // clamped to [0, size). For closed paths, the index is wrapped, such that
  // EffectiveIndex(-1) = Size() - 1.
  int EffectiveIndex(int n) const { return EffectiveIndex(n, Size(), Closed()); }
  // Equivalent to Point(EffectiveIndex(n)).
  QPoint EffectivePoint(int n) const { return Point(EffectiveIndex(n)); }

  // Generates the sequence of points to render as part of the path.
  QVector<QPoint> RenderPoints(const QVector<QPoint> &user_points) const;

  QPoint mousePos;
  int selectedPointIndex = -1;

 protected:
  ProtoModelPtr pathModel;
  // Used internally to reduce the number of proto model interactions.
  int EffectiveIndex(int n, int size, bool closed) const;
  // Used internally to reduce the number of proto model interactions.
  QPoint EffectivePoint(int n, int size, bool closed) const;
};

#endif  // PATHVIEW_H
