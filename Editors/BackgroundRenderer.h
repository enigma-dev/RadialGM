#ifndef BACKGROUNDRENDERER_H
#define BACKGROUNDRENDERER_H

#include <QObject>
#include <QWidget>

class BackgroundRenderer : public QWidget
{
    Q_OBJECT
public:
    explicit BackgroundRenderer(QWidget *parent = nullptr);
    QSize sizeHint() const override;
    void setImage(QString imageFile);
    void setGrid(bool visible, unsigned hOff, unsigned vOff, unsigned w, unsigned h, unsigned hSpacing, unsigned vSpacing);

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:

private:
    QPixmap image;
    bool grid_visible;
    unsigned grid_hOff;
    unsigned grid_vOff;
    unsigned grid_w;
    unsigned grid_h;
    unsigned grid_hSpacing;
    unsigned grid_vSpacing;
};

#endif // BACKGROUNDRENDERER_H
