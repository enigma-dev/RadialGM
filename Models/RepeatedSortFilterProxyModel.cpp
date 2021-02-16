#include "RepeatedSortFilterProxyModel.h"
#include "Components/Logger.h"

RepeatedSortFilterProxyModel::RepeatedSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {}

void RepeatedSortFilterProxyModel::SetSourceModel(RepeatedModel *sourceModel) {
  model_ = sourceModel;
  QSortFilterProxyModel::setSourceModel(model_);
}

QVariant RepeatedSortFilterProxyModel::Data(FieldPath field_path) const { return DataOrDefault(field_path); }

QVariant RepeatedSortFilterProxyModel::DataOrDefault(FieldPath field_path, const QVariant def) const {
  R_EXPECT(model_, QVariant()) << "Internal model null";

  auto idx = index(field_path.repeated_field_index, 0);

  R_EXPECT(field_path.repeated_field_index != -1 && idx.isValid() && idx.internalPointer(), QVariant())
      << "Invalid index" << idx;

  int source_row = mapToSource(idx).row();
  return model_->DataOrDefault(FieldPath{source_row, field_path.fields}, def);
}

ProtoModel* RepeatedSortFilterProxyModel::GetSubModel(int fieldNum) const {
  R_EXPECT(model_, nullptr) << "Internal model null";
  int source_row = mapToSource(index(fieldNum, 0)).row();
  return model_->GetSubModel(source_row);
}
