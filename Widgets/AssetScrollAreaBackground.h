#ifndef ASSETSCROLLAREABACKGROUND_H
#define ASSETSCROLLAREABACKGROUND_H

#include "AssetScrollArea.h"

#include <QSet>
#include <QWidget>

#include <cmath>

inline bool compareDouble(double a, double b) { return std::fabs(a - b) < std::numeric_limits<double>::epsilon(); }

// RoundNum round to the nearest multiple
template <typename T>
T RoundNum(T value, T multiple) {
  if (compareDouble(multiple, 0)) return value;
  return static_cast<T>(std::round(static_cast<double>(value) / static_cast<double>(multiple)) *
                        static_cast<double>(multiple));
}

class AssetView;

class AssetScrollAreaBackground : public QWidget {
  Q_OBJECT
 public:
  explicit AssetScrollAreaBackground(AssetScrollArea* parent = nullptr);
  ~AssetScrollAreaBackground();
  void SetAssetView(AssetView* asset);
  void SetDrawSolidBackground(bool b, QColor color);
  const qreal& GetZoom() const;

  bool parentHasFocus = false;

 public slots:
  void SetZoom(qreal zoom);

 signals:
  void MouseMoved(int x, int y);
  void MousePressed(Qt::MouseButton button);
  void MouseReleased(Qt::MouseButton button);

 protected:
  // Standard Grid
  void paintGrid(QPainter& painter, int gridHorSpacing, int gridVertSpacing, int gridHorOff, int gridVertOff);
  // Grid used in tilesets
  void paintGrid(QPainter& painter, int width, int height, int gridHorSpacing, int gridVertSpacing, int gridHorOff,
                 int gridVertOff, int gridWidth, int gridHeight);
  void paintEvent(QPaintEvent* event) override;
  bool eventFilter(QObject* obj, QEvent* event) override;

  AssetView* assetView = nullptr;
  bool drawSolidBackground = true;
  qreal zoom = 1;
  QPoint offset;
  QPoint userOffset;
  QSet<int> pressedKeys;
  QColor backgroundColor = Qt::GlobalColor::gray;
};

#endif  // ASSETSCROLLAREABACKGROUND_H