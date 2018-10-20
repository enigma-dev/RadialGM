#include "ColorPicker.h"

#include <QColorDialog>
#include <QHBoxLayout>
#include <QPainter>

ColorPicker::ColorPicker(QWidget *parent) : QWidget(parent) {
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0);
  button = new QToolButton(this);
  layout->addWidget(button);
  this->setLayout(layout);

  connect(button, &QToolButton::clicked, [=]() {
    QColor color = QColorDialog::getColor(color_, this, tr("Choose a color"), QColorDialog::ShowAlphaChannel);
    // don't accept the color if the user cancelled the dialog
    if (!color.isValid()) return;
    this->setColor(color);
  });
  connect(this, &ColorPicker::colorChanged, this, &ColorPicker::updateButtonIcon);
  updateButtonIcon();
}

QColor ColorPicker::color() const { return color_; }

void ColorPicker::setColor(QColor color) {
  if (color_ == color) return;
  color_ = color;
  emit colorChanged(color);
}

void ColorPicker::updateButtonIcon() {
  QIcon transparencyIcon(":/transparent.png");
  QPixmap pixmap = transparencyIcon.pixmap(24, 24);
  QPainter painter(&pixmap);
  painter.fillRect(pixmap.rect(), QBrush(color_));
  button->setIcon(pixmap);
}
