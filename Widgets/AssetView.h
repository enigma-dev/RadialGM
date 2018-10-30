#ifndef ASSETVIEW_H
#define ASSETVIEW_H

#include <QPainter>
#include <QWidget>

class AssetView : public QWidget {
  Q_OBJECT
 public:
  explicit AssetView(QWidget* parent = nullptr);

  void paintGrid(QPainter& painter, int width, int height, int gridHorSpacing, int gridVertSpacing, int gridHorOff,
                 int gridVertOff, int gridWidth, int gridHeight);
  const qreal& GetZoom() const;

 protected:
  qreal zoom;
};

#endif  // ASSETVIEW_H
