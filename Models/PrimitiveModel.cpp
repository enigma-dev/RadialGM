#include "PrimitiveModel.h"

#include <Components/ArtManager.h>
#include <QIcon>

const ProtoModel *PrimitiveModel::GetSubModel(const FieldPath &field_path) const {
  if (field_path) {
    qDebug() << "Trying to access a field within a primitive field...";
    return nullptr;
  }
  return this;
}

const FieldDescriptor *PrimitiveModel::GetRowDescriptor(int row) const {
  Q_UNUSED(row);  // All rows of a repeated field have the same descriptor.
  if (field_or_null_) return field_or_null_;
  return _parentModel->GetRowDescriptor(row_in_parent_);
}

QIcon PrimitiveModel::GetDisplayIcon() const {
  const FieldDescriptor *field = GetRowDescriptor(0);
  auto &display = GetFieldDisplay(field->full_name());
  if (display.icon_lookup_function) return display.icon_lookup_function(GetDirect());
  if (!display.default_icon_name.isEmpty()) return ArtManager::GetIcon(display.default_icon_name);
  return {};
}
