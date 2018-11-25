#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QToolButton>
#include <QWidget>

class ColorPicker : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged USER true)
  Q_PROPERTY(bool alphaEnabled READ alphaEnabled WRITE setAlphaEnabled)

 public:
  explicit ColorPicker(QWidget *parent = nullptr);

  QColor color() const;
  void setColor(const QColor &);
  bool alphaEnabled() const;
  void setAlphaEnabled(bool enabled);

 signals:
  void colorChanged(QColor);

 private slots:
  void updateButtonIcon();

 private:
  QColor color_;
  bool alpha_enabled_;

  QToolButton *button;
};

#endif  // COLORPICKER_H
