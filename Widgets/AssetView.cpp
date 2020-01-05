#include "AssetView.h"

AssetView::AssetView(AssetScrollAreaBackground* parent) : QWidget(parent) { setMouseTracking(true); }

GridDimensions& AssetView::GetGrid() { return grid; }

void AssetView::paintEvent(QPaintEvent* /* event */) {
  // We don't want to paint here because painting is handled by AssetScrollAreaBackground
  // QPainter painter(this);
  // Paint(painter);
}
