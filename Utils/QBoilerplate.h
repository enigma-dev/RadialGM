#ifndef QBOILERPLATE_H
#define QBOILERPLATE_H

#include <QStyledItemDelegate>

namespace qboilerplate {

class RemoveDisplayRole : public QStyledItemDelegate {
  public:
    using QStyledItemDelegate::QStyledItemDelegate;

  protected:
    void initStyleOption(QStyleOptionViewItem *o, const QModelIndex &idx) const override {
      QStyledItemDelegate::initStyleOption(o, idx);
      o->features &= ~QStyleOptionViewItem::HasDisplay;
    }
};

}  // namespace qboilerplate

#endif  // QBOILERPLATE_H
