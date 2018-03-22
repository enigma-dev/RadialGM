#ifndef BACKGROUNDRENDERER_H
#define BACKGROUNDRENDERER_H

#include <QObject>
#include <QWidget>

class BackgroundRenderer : public QWidget {
  Q_OBJECT
 public:
  explicit BackgroundRenderer(QWidget *parent);
  QSize sizeHint() const override;
  void setImage(QPixmap image);
  void setGrid(bool visible, int hOff, int vOff, int w, int h, int hSpacing, int vSpacing);
  void setTransparent(bool transparent);
  void setZoom(qreal zoom);
  const qreal &getZoom() const;

 protected:
  void paintEvent(QPaintEvent *event) override;

 public slots:

 private:
  QPixmap pixmap;
  QPixmap transparentPixmap;
  QColor transparencyColor;
  bool transparent;
  bool grid_visible;
  int grid_hOff;
  int grid_vOff;
  int grid_w;
  int grid_h;
  int grid_hSpacing;
  int grid_vSpacing;
  qreal zoom;
};

#endif  // BACKGROUNDRENDERER_H
