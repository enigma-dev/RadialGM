#include "PrimitiveModel.h"
#include "MainWindow.h"

#include <Components/ArtManager.h>
#include <QIcon>

PrimitiveModel::PrimitiveModel(MessageModel *parent, const FieldDescriptor *field)
    : ProtoModel(parent, parent->GetDescriptor()->name(), parent->GetDescriptor(), field->index()),
      field_or_null_(field) {
  if (field_or_null_ && !field_or_null_->options().GetExtension(buffers::resource_ref).empty())
    connect(MainWindow::resourceMap.get(),
          qOverload<const std::string &, const QString &, const QString &>(&ResourceModelMap::ResourceRenamed), this,
          &PrimitiveModel::ResourceRenamed, Qt::DirectConnection);
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
  if (field_or_null_) return QString::fromStdString(field_or_null_->name());
  if (const auto *fd = _parentModel->GetRowDescriptor(row_in_parent_)) return QString::fromStdString(fd->full_name());
  return "Error";
}

QIcon PrimitiveModel::GetDisplayIcon() const {
  const FieldDescriptor *field = GetRowDescriptor(0);
  auto &display = GetFieldDisplay(field->full_name());
  QIcon ret;
  if (display.icon_lookup_function) ret = display.icon_lookup_function(GetDirect());
  if (!ret.isNull()) return ret;
  if (!display.default_icon_name.isEmpty()) return ArtManager::GetIcon(display.default_icon_name);
  return {};
}

void PrimitiveModel::ResourceRenamed(const std::string &type, const QString &oldName, const QString &newName) {
  if (field_or_null_ && field_or_null_->options().GetExtension(buffers::resource_ref) == type) {
    if (Data().toString() == oldName) SetData(newName);
  }
}
