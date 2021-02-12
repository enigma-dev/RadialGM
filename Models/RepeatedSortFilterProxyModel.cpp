#include "RepeatedSortFilterProxyModel.h"

RepeatedSortFilterProxyModel::RepeatedSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {}

void RepeatedSortFilterProxyModel::SetSourceModel(RepeatedModel *sourceModel) {
  model_ = sourceModel;
  QSortFilterProxyModel::setSourceModel(model_);
}

QVariant RepeatedSortFilterProxyModel::Data(FieldPath field_path) const {
  if (!model_) {
    qDebug() << "null model";
    return QVariant();
  }
  auto idx = index(field_path.repeated_field_index, 0);
  if (field_path.repeated_field_index == -1 || !idx.isValid() || !idx.internalPointer()) {
    qDebug() << "invalid index";
    return QVariant();
  }
  int source_row = mapToSource(idx).row();
  return model_->Data(FieldPath{source_row, field_path.fields});
}

QVariant RepeatedSortFilterProxyModel::DataOrDefault(FieldPath field_path) const {
  return 1;
}

void RepeatedSortFilterProxyModel::sort(int column, Qt::SortOrder order) {
  if (!model_) {
    qDebug() << "null model";
    return;
  }
  if (auto *const repeated_message_model = model_->TryCastAsRepeatedMessageModel()) {
    QSortFilterProxyModel::sort(repeated_message_model->FieldToColumn(column), order);
  } else
    QSortFilterProxyModel::sort(column, order);
}
