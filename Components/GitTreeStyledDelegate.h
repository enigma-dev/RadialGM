#ifndef GITTREESTYLEDDELEGATE_H
#define GITTREESTYLEDDELEGATE_H

#include <QStyledItemDelegate>

class GitTreeStyledDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
  GitTreeStyledDelegate(QObject* parent = nullptr);
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // GITTREESTYLEDDELEGATE_H
