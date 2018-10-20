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
    QColor color = QColorDialog::getColor(this->color_, this, tr("Choose a color"), QColorDialog::ShowAlphaChannel);
    if (!color.isValid()) return;
    this->setColor(color);
  });
  connect(this, &ColorPicker::colorChanged, this, &ColorPicker::updateButtonIcon);
  this->updateButtonIcon();
}

QColor ColorPicker::color() const { return this->color_; }

void ColorPicker::setColor(QColor color) {
  if (this->color_ == color) return;
  this->color_ = color;
  emit colorChanged(color);
}

void ColorPicker::updateButtonIcon() {
  QIcon transparencyIcon(":/transparent.png");
  QPixmap pixmap = transparencyIcon.pixmap(24, 24);
  QPainter painter(&pixmap);
  painter.fillRect(pixmap.rect(), QBrush(this->color_));
  button->setIcon(pixmap);
}
