#ifndef REPEATEDSORTFILTERPROXYMODEL_H
#define REPEATEDSORTFILTERPROXYMODEL_H

#include "Models/RepeatedModel.h"
#include "Models/RepeatedMessageModel.h"
#include "Utils/FieldPath.h"

#include <QSortFilterProxyModel>

class RepeatedSortFilterProxyModel : public QSortFilterProxyModel {
 public:
  RepeatedSortFilterProxyModel(QObject *parent);
  void SetSourceModel(RepeatedModel *sourceModel);
  QVariant Data(FieldPath field_path) const;
  QVariant DataOrDefault(FieldPath field_path, const QVariant def = QVariant()) const;
  void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

protected:
  void setSourceModel(QAbstractItemModel* /*sourceModel*/) override {}
  RepeatedModel* model_;
};

#endif  // REPEATEDSORTFILTERPROXYMODEL_H
