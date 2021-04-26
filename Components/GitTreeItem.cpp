#include "GitTreeItem.h"
#include <QDebug>

GitTreeItem::GitTreeItem() {}

constexpr int scale = 8;

void GitTreeItem::paint(QPainter *painter, const QRect &rect, const QPalette &palette, EditMode mode) const {
  painter->save();
  painter->setBrush(QBrush(Qt::black));
  painter->drawEllipse(rect.left() + (rect.width() - scale)/2, rect.top() + (rect.height() - scale)/2, scale, scale);
  painter->drawLine(rect.left() + (rect.width())/2, rect.top(), rect.left() + (rect.width())/2, rect.bottom());
  painter->restore();
}

QSize GitTreeItem::sizeHint() const {
  return QSize(scale, scale);
}
