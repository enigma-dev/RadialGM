#ifndef GITTREEITEM_H
#define GITTREEITEM_H

#include <QPainter>
#include <QSize>

class GitTreeItem
{
public:
  enum class EditMode { ReadOnly };
  GitTreeItem();
  void paint(QPainter *painter, const QRect &rect, const QPalette &palette, EditMode mode) const;
  QSize sizeHint() const;
};

Q_DECLARE_METATYPE(GitTreeItem)

#endif // GITTREEITEM_H
