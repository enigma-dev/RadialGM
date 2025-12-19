#include "PrimitiveModel.h"
#include "MainWindow.h"

#include <Components/ArtManager.h>
#include <QIcon>

PrimitiveModel::PrimitiveModel(MessageModel *parent, const FieldDescriptor *field)
    : ProtoModel(parent, std::string(parent->GetDescriptor()->name()), parent->GetDescriptor(), field->index()),
      field_or_null_(field) {
  ProtoModel* p = this;
  while (p) {
    emit p->ModelConstructed(this);
    p = p->GetParentModel<ProtoModel*>();
  }
}

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

QString PrimitiveModel::GetDisplayName() const {
  if (field_or_null_) {
    auto name_sv = field_or_null_->name();
    return QString::fromUtf8(name_sv.data(), name_sv.size());
  }
  if (const auto *fd = _parentModel->GetRowDescriptor(row_in_parent_)) {
    auto full_name_sv = fd->full_name();
    return QString::fromUtf8(full_name_sv.data(), full_name_sv.size());
  }
  return "Error";
}

QIcon PrimitiveModel::GetDisplayIcon() const {
  const FieldDescriptor *field = GetRowDescriptor(0);
  auto &display = GetFieldDisplay(std::string(field->full_name()));
  QIcon ret;
  if (display.icon_lookup_function) ret = display.icon_lookup_function(GetDirect());
  if (!ret.isNull()) return ret;
  if (!display.default_icon_name.isEmpty()) return ArtManager::GetIcon(display.default_icon_name);
  return {};
}
