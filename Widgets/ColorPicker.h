#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QToolButton>
#include <QWidget>

class ColorPicker : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged USER true)
 public:
  explicit ColorPicker(QWidget *parent = nullptr);

  QColor color() const;
  void setColor(QColor);

 signals:
  void colorChanged(QColor);

 private slots:
  void updateButtonIcon();

 private:
  QColor color_;

  QToolButton *button;
};

#endif  // COLORPICKER_H
