#include "AssetView.h"

AssetView::AssetView(AssetScrollAreaBackground* parent) : QWidget(parent), _parent(parent) { setMouseTracking(true); }

GridDimensions& AssetView::GetGrid() { return _grid; }

QRect AssetView::getVisibleHint() {
  return _visible;
}

void AssetView::setVisibleHint(QRect visible) {
  _visible = visible;
}

void AssetView::paintEvent(QPaintEvent* /* event */) {
  // We don't want to paint here because painting is handled by AssetScrollAreaBackground
  // QPainter painter(this);
  // Paint(painter);
}
