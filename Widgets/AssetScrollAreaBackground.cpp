#include "AssetScrollAreaBackground.h"
#include "Components/ArtManager.h"
#include "MainWindow.h"
#include "Widgets/RoomView.h"

#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

AssetScrollAreaBackground::AssetScrollAreaBackground(AssetScrollArea* parent)
    : QWidget(parent),
      _assetView(nullptr),
      _drawSolidBackground(true),
      _currentZoom(1),
      _zoomFactor(2),
      _maxZoom(3200),
      _minZoom(0.0625),
      _backgroundColor(Qt::GlobalColor::gray),
      _viewMoveSpeed(4) {
  installEventFilter(this);
  setMouseTracking(true);
  // Redraw on an model changes
  connect(MainWindow::resourceMap.get(), &ResourceModelMap::DataChanged, this, [this]() { this->update(); });
}

AssetScrollAreaBackground::~AssetScrollAreaBackground() {
  disconnect(MainWindow::resourceMap.get(), &ResourceModelMap::DataChanged, this, nullptr);
}

void AssetScrollAreaBackground::SetAssetView(AssetView* asset) {
  _assetView = asset;
  SetZoom(1);
}

void AssetScrollAreaBackground::SetDrawSolidBackground(bool b, QColor color) {
  _drawSolidBackground = b;
  _backgroundColor = color;
  update();
}

void AssetScrollAreaBackground::SetZoom(qreal zoom) {
  if (zoom > 3200) zoom = _maxZoom;
  if (zoom < 0.0625) zoom = _minZoom;
  this->_currentZoom = zoom;

  if (_assetView != nullptr) _assetView->setFixedSize(_assetView->sizeHint() * zoom);

  update();
}

void AssetScrollAreaBackground::SetZoomRange(qreal scalingFactor, qreal min, qreal max) {
  _zoomFactor = scalingFactor;
  _minZoom = min;
  _maxZoom = max;
}

bool AssetScrollAreaBackground::GetGridVisible() {
  if (_assetView != nullptr) {
    return _assetView->GetGrid().show;
  }
  return false;
}

void AssetScrollAreaBackground::ResetZoom() { SetZoom(1); }

void AssetScrollAreaBackground::ZoomIn() { SetZoom(GetZoom() * _zoomFactor); }

void AssetScrollAreaBackground::ZoomOut() { SetZoom(GetZoom() / _zoomFactor); }

const qreal& AssetScrollAreaBackground::GetZoom() const { return _currentZoom; }

void AssetScrollAreaBackground::SetGridVisible(bool visible) {
  if (_assetView != nullptr) {
    _assetView->GetGrid().show = visible;
    update();
  }
}

void AssetScrollAreaBackground::SetGridHSnap(int hSnap) {
  if (_assetView != nullptr) {
    _assetView->GetGrid().horSpacing = hSnap;
  }
}

void AssetScrollAreaBackground::SetGridVSnap(int vSnap) {
  if (_assetView != nullptr) {
    _assetView->GetGrid().vertSpacing = vSnap;
  }
}

void AssetScrollAreaBackground::PaintGrid(QPainter& painter, int gridHorSpacing, int gridVertSpacing, int gridHorOff,
                                          int gridVertOff) {
  // save the painter state so we can restore it before returning
  painter.save();

  // xor the destination color
  painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
  // this pen not only sets the color but also gives us perfectly square rectangles
  QPen pen(Qt::white, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
  painter.setPen(pen);

  if (gridHorSpacing != 0) {
    for (int x = gridHorOff + gridHorSpacing; x < width(); x += gridHorSpacing) {
      painter.drawLine(x, 0, x, this->height());
    }

    for (int x = gridHorOff; x > 0; x -= gridHorSpacing) {
      painter.drawLine(x, 0, x, this->height());
    }
  }

  if (gridVertSpacing != 0) {
    for (int y = gridVertOff + gridVertSpacing; y < height(); y += gridVertSpacing) {
      painter.drawLine(0, y, this->width(), y);
    }

    for (int y = gridVertOff; y > 0; y -= gridVertSpacing) {
      painter.drawLine(0, y, this->width(), y);
    }
  }

  painter.restore();
}

void AssetScrollAreaBackground::PaintGrid(QPainter& painter, int width, int height, int gridHorSpacing,
                                          int gridVertSpacing, int gridHorOff, int gridVertOff, int gridWidth,
                                          int gridHeight) {
  // do not draw zero-area grids
  if (width == 0 || height == 0) return;
  if (gridWidth == 0 || gridHeight == 0) return;

  // save the painter state so we can restore it before returning
  painter.save();

  // xor the destination color
  painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
  // this pen not only sets the color but also gives us perfectly square rectangles
  QPen pen(Qt::white, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
  painter.setPen(pen);

  if (gridHorSpacing != 0 || gridVertSpacing != 0) {
    painter.translate(0.5, 0.5);
    const int horStep = gridWidth + gridHorSpacing;
    const int verStep = gridHeight + gridVertSpacing;

    for (int x = gridHorOff; x < width + gridHorOff; x += horStep) {
      for (int y = gridVertOff; y < height + gridVertOff; y += verStep) {
        painter.drawRect(x, y, gridWidth - 1, gridHeight - 1);
      }
    }
  } else {
    for (int x = gridHorOff; x <= width + gridHorOff; x += gridWidth)
      painter.drawLine(x, gridVertOff, x, gridVertOff + height);
    for (int y = gridVertOff; y <= height + gridVertOff; y += gridHeight)
      painter.drawLine(gridHorOff, y, gridHorOff + width, y);
  }

  painter.restore();
}

QPoint AssetScrollAreaBackground::GetCenterOffset() {
  return QPoint(
      (rect().width() < _assetView->rect().width()) ? 0 : rect().center().x() - _assetView->rect().center().x(),
      (rect().height() < _assetView->rect().height()) ? 0 : rect().center().y() - _assetView->rect().center().y());
}

void AssetScrollAreaBackground::paintEvent(QPaintEvent* /* event */) {
  QPainter painter(this);

  if (_drawSolidBackground) {
    painter.fillRect(painter.viewport(), _backgroundColor);
  } else {
    painter.save();
    painter.scale(4, 4);
    painter.fillRect(painter.viewport(), ArtManager::GetTransparenyBrush());
    painter.restore();
  }

  if (_assetView != nullptr) {
    _totalDrawOffset = GetCenterOffset() + _userDrawOffset;

    painter.save();
    painter.translate(_totalDrawOffset);
    painter.scale(_currentZoom, _currentZoom);
    _assetView->Paint(painter);
    painter.restore();

    GridDimensions g = _assetView->GetGrid();
    if (g.show) {
      if (g.type == GridType::Complex)
        PaintGrid(painter, g.width * _currentZoom, g.height * _currentZoom, g.horSpacing * _currentZoom,
                  g.vertSpacing * _currentZoom, (g.horOff * _currentZoom) + _totalDrawOffset.x(),
                  (g.vertOff * _currentZoom) + _totalDrawOffset.y(), g.cellWidth * _currentZoom,
                  g.cellHeight * _currentZoom);
      else if (g.type == GridType::Standard)
        PaintGrid(painter, g.horSpacing * _currentZoom, g.vertSpacing * _currentZoom, _totalDrawOffset.x(),
                  _totalDrawOffset.y());
    }
  }

  _assetView->PaintTop(painter);
}

bool AssetScrollAreaBackground::eventFilter(QObject* obj, QEvent* event) {
  switch (event->type()) {
    case QEvent::MouseMove: {
      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
      QPoint roomPos = mouseEvent->pos() - _totalDrawOffset;
      roomPos /= _currentZoom;
      emit MouseMoved(roomPos.x(), roomPos.y());
      break;
    }
    case QEvent::MouseButtonPress: {
      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
      emit MousePressed(mouseEvent->button());
      break;
    }
    case QEvent::MouseButtonRelease: {
      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
      emit MouseReleased(mouseEvent->button());
      break;
    }
    case QEvent::KeyPress: {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
      _pressedKeys += keyEvent->key();
      _userDrawOffset.setX(_userDrawOffset.x() +
                           (_pressedKeys.contains(Qt::Key::Key_A) - _pressedKeys.contains(Qt::Key::Key_D)) *
                               _viewMoveSpeed);
      _userDrawOffset.setY(_userDrawOffset.y() +
                           (_pressedKeys.contains(Qt::Key::Key_W) - _pressedKeys.contains(Qt::Key::Key_S)) *
                               _viewMoveSpeed);
      update();
      break;
    }
    case QEvent::KeyRelease: {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
      _pressedKeys -= keyEvent->key();
      break;
    }
    default: break;
  }
  return QWidget::eventFilter(obj, event);
}
