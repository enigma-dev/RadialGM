#ifndef ASSETVIEW_H
#define ASSETVIEW_H

#include "AssetScrollAreaBackground.h"

#include <QPainter>
#include <QWidget>

enum GridType {
  Standard,   // Simple x by x Grid
  Complex,    // Grid with offset and spacing between squares
  Isometric,  // TODO: Implement this
};

struct GridDimensions {
  GridType type = GridType::Standard;
  bool show = true;
  int horSpacing = 16;
  int vertSpacing = 16;
  int horOff = 0;
  int vertOff = 0;
  int cellWidth = 0;
  int cellHeight = 0;
  int width = 0;
  int height = 0;
};

class AssetView : public QWidget {
  Q_OBJECT
 public:
  explicit AssetView(AssetScrollAreaBackground* parent);
  virtual void Paint(QPainter& painter) = 0;
  GridDimensions& GetGrid();

 protected:
  void paintEvent(QPaintEvent* event);

  GridDimensions grid;
};

#endif  // ASSETVIEW_H
