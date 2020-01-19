#include "ColorPicker.h"

#include <QColorDialog>
#include <QHBoxLayout>
#include <QPainter>

ColorPicker::ColorPicker(QWidget *parent) : QWidget(parent), _color(Qt::black), _alpha_enabled(true) {
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0);
  _button = new QToolButton(this);
  layout->addWidget(_button);
  this->setLayout(layout);

  connect(_button, &QToolButton::clicked, [=]() {
    QColorDialog::ColorDialogOptions options;
    if (_alpha_enabled) options |= QColorDialog::ShowAlphaChannel;
    QColor color = QColorDialog::getColor(_color, this, tr("Choose a color"), options);
    // don't accept the color if the user cancelled the dialog
    if (!color.isValid()) return;
    this->setColor(color);
  });
  connect(this, &ColorPicker::colorChanged, this, &ColorPicker::updateButtonIcon);
  updateButtonIcon();
}

QColor ColorPicker::color() const { return _color; }

void ColorPicker::setColor(const QColor &color) {
  if (_color == color) return;
  _color = color;
  emit colorChanged(color);
}

void ColorPicker::updateButtonIcon() {
  QIcon transparencyIcon(":/transparent.png");
  QPixmap pixmap = transparencyIcon.pixmap(24, 24);
  QPainter painter(&pixmap);
  painter.fillRect(pixmap.rect(), QBrush(_color));
  _button->setIcon(pixmap);
}

bool ColorPicker::alphaEnabled() const { return _alpha_enabled; }

void ColorPicker::setAlphaEnabled(bool enabled) { _alpha_enabled = enabled; }
