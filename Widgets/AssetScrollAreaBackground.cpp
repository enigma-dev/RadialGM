#include "AssetScrollAreaBackground.h"
#include "Components/ArtManager.h"
#include "MainWindow.h"
#include "Widgets/RoomView.h"

#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

AssetScrollAreaBackground::AssetScrollAreaBackground(AssetScrollArea* parent) : QWidget(parent) {
  installEventFilter(this);
  setMouseTracking(true);
  // Redraw on an model changes
  connect(MainWindow::resourceMap.get(), &ResourceModelMap::dataChanged, this, [this]() { this->update(); });
}

AssetScrollAreaBackground::~AssetScrollAreaBackground() {
  disconnect(MainWindow::resourceMap.get(), &ResourceModelMap::dataChanged, this, nullptr);
}

void AssetScrollAreaBackground::SetAssetView(AssetView* asset) {
  assetView = asset;
  SetZoom(1);
}

void AssetScrollAreaBackground::SetDrawSolidBackground(bool b, QColor color) {
  drawSolidBackground = b;
  backgroundColor = color;
  update();
}

void AssetScrollAreaBackground::SetZoom(qreal zoom) {
  if (zoom > 3200) zoom = 3200;
  if (zoom < 0.0625) zoom = 0.0625;
  this->zoom = zoom;

  if (assetView != nullptr) assetView->setFixedSize(assetView->sizeHint() * zoom);

  update();
}

const qreal& AssetScrollAreaBackground::GetZoom() const { return zoom; }

void AssetScrollAreaBackground::paintGrid(QPainter& painter, int gridHorSpacing, int gridVertSpacing, int gridHorOff,
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

void AssetScrollAreaBackground::paintGrid(QPainter& painter, int width, int height, int gridHorSpacing,
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

void AssetScrollAreaBackground::paintEvent(QPaintEvent* /* event */) {
  QPainter painter(this);

  if (drawSolidBackground) {
    painter.fillRect(painter.viewport(), backgroundColor);
  } else {
    painter.save();
    painter.scale(4, 4);
    painter.fillRect(painter.viewport(), ArtManager::GetTransparenyBrush());
    painter.restore();
  }

  if (assetView != nullptr) {
    offset =
        QPoint(
            (rect().width() < assetView->rect().width()) ? 0 : rect().center().x() - assetView->rect().center().x(),
            (rect().height() < assetView->rect().height()) ? 0 : rect().center().y() - assetView->rect().center().y()) +
        userOffset;

    painter.save();
    painter.translate(offset);
    painter.scale(zoom, zoom);
    assetView->Paint(painter);
    painter.restore();

    GridDimensions g = assetView->GetGrid();
    if (g.show) {
      if (g.type == GridType::Complex)
        paintGrid(painter, g.width * zoom, g.height * zoom, g.horSpacing * zoom, g.vertSpacing * zoom,
                  (g.horOff * zoom) + offset.x(), (g.vertOff * zoom) + offset.y(), g.cellWidth * zoom,
                  g.cellHeight * zoom);
      else if (g.type == GridType::Standard)
        paintGrid(painter, g.horSpacing * zoom, g.vertSpacing * zoom, offset.x(), offset.y());
    }
  }
}

bool AssetScrollAreaBackground::eventFilter(QObject* obj, QEvent* event) {
  if (parentHasFocus) {
    switch (event->type()) {
      case QEvent::MouseMove: {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint roomPos = mouseEvent->pos() - offset;
        roomPos /= zoom;
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
      case QEvent::Enter: {
        setFocus();
        break;
      }
      case QEvent::Leave: {
        clearFocus();
        break;
      }
      case QEvent::KeyPress: {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        pressedKeys += keyEvent->key();
        userOffset.setX(userOffset.x() +
                        (pressedKeys.contains(Qt::Key::Key_D) - pressedKeys.contains(Qt::Key::Key_A)) * 4);
        userOffset.setY(userOffset.y() +
                        (pressedKeys.contains(Qt::Key::Key_W) - pressedKeys.contains(Qt::Key::Key_S)) * 4);
        update();
        break;
      }
      case QEvent::KeyRelease: {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        pressedKeys -= keyEvent->key();
        break;
      }
      default: break;
    }
  }
  return QWidget::eventFilter(obj, event);
}
