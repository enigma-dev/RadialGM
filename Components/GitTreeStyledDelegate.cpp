#include "GitTreeStyledDelegate.h"
#include "GitTreeItem.h"

#include <QPainter>

GitTreeStyledDelegate::GitTreeStyledDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

void GitTreeStyledDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  if (index.data().canConvert<GitTreeItem>()) {
    GitTreeItem i = qvariant_cast<GitTreeItem>(index.data());
    i.paint(painter, option.rect, option.palette, GitTreeItem::EditMode::ReadOnly);
  } else QStyledItemDelegate::paint(painter, option, index);
}

QSize GitTreeStyledDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
  if (index.data().canConvert<GitTreeItem>()) {
    GitTreeItem i = qvariant_cast<GitTreeItem>(index.data());
    return i.sizeHint();
  } else return QStyledItemDelegate::sizeHint(option, index);
}
