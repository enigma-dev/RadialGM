#include "ProtoModel.h"
#include "MessageModel.h"
#include "RepeatedMessageModel.h"

#include "Components/Logger.h"

#include <Components/ArtManager.h>
#include <QIcon>

ProtoModel::DisplayConfig ProtoModel::display_config_;

ProtoModel::ProtoModel(NonProtoParent parent, std::string name, const Descriptor *descriptor)
    : ProtoModel(static_cast<ProtoModel *>(nullptr), name, descriptor, -1) {
  QObject::setParent(parent.parent);
}

ProtoModel::ProtoModel(ProtoModel *parent, std::string name, const Descriptor *descriptor, int row_in_parent)
    : QAbstractItemModel(parent),
      _dirty(false),
      _parentModel(parent),
      row_in_parent_(row_in_parent),
      debug_path_((parent ? parent->debug_path_ + "." : "") + QString::fromStdString(name)),
      descriptor_(descriptor) {
  connect(this, &ProtoModel::DataChanged, this,
          [this](const QModelIndex &topLeft, const QModelIndex &bottomRight,
                 const QVariant & /*oldValue*/ = QVariant(0), const QVector<int> &roles = QVector<int>()) {
            emit QAbstractItemModel::dataChanged(topLeft, bottomRight, roles);
          });
  if (parent) {
    connect(this, &ProtoModel::dataChanged, this, [this](const QModelIndex&, const QModelIndex&, const QVector<int>&) {
      auto me = _parentModel->index(row_in_parent_);
      emit _parentModel->dataChanged(me, me, {});
    });
    connect(this, &ProtoModel::modelReset, this, [this]() {
      auto me = _parentModel->index(row_in_parent_);
      emit _parentModel->dataChanged(me, me, {});
    });
  }
}

void ProtoModel::ParentDataChanged() {
  ProtoModel *m = GetParentModel<ProtoModel *>();
  while (m != nullptr) {
    emit m->DataChanged(m->index(0, 0), m->index(rowCount() - 1, columnCount() - 1));
    m = m->GetParentModel<ProtoModel *>();
  }
}

void ProtoModel::SetDirty(bool dirty) { _dirty = dirty; }

bool ProtoModel::IsDirty() { return _dirty; }

QIcon LookUpIconByName(const QVariant &name) { return ArtManager::GetIcon(name.toString()); }

void ProtoModel::DisplayConfig::SetDefaultIcon(const std::string &message, const QString &icon_name) {
  message_display_configs_[message].default_icon_name = icon_name;
}
void ProtoModel::DisplayConfig::SetMessageIconPathField(const std::string &message, const FieldPath &field_path) {
  SetMessageIconIdField(message, field_path, LookUpIconByName);
}
void ProtoModel::DisplayConfig::SetMessageLabelField(const std::string &message, const FieldPath &field_path) {
  message_display_configs_[message].label_field = field_path;
}
void ProtoModel::DisplayConfig::SetMessageIconIdField(const std::string &message, const FieldPath &field_path,
                                                      FieldDisplayConfig::IconLookupFn icon_lookup_function) {
  std::string field = message;
  message_display_configs_[message].icon_field = field_path;
  for (const auto &fcomp : field_path.fields) field += "." + fcomp->name();
  if (field_path.size() != 1)
    qDebug() << "Warning: Nested icon fields not currently implemented; `" << field.c_str() << "` won't work properly";
  field_display_configs_[field].icon_lookup_function = icon_lookup_function;
}

void ProtoModel::DisplayConfig::SetFieldIconLookup(const FieldDescriptor *field,
                                                   FieldDisplayConfig::IconLookupFn icon_lookup_function) {
  field_display_configs_[field->full_name()].icon_lookup_function = icon_lookup_function;
}

void ProtoModel::DisplayConfig::SetFieldDefaultIcon(const FieldDescriptor *field, const QString &icon_name) {
  field_display_configs_[field->full_name()].default_icon_name = icon_name;
}

void ProtoModel::DisplayConfig::SetFieldHeaderIcon(const std::string &message, const FieldPath &field_path,
                                                   const QString &icon_name) {
  std::string field = message;
  message_display_configs_[message].icon_field = field_path;
  for (const auto &fcomp : field_path.fields) field += "." + fcomp->name();
  if (field_path.size() != 1)
    qDebug() << "Warning: Nested icon fields not currently implemented; `" << field.c_str() << "` won't work properly";
  field_display_configs_[field].header_icon = icon_name;
}

const ProtoModel::FieldDisplayConfig &ProtoModel::DisplayConfig::GetFieldDisplay(const std::string &field_qname) const {
  static const ProtoModel::FieldDisplayConfig sentinel(false);
  if (auto it = field_display_configs_.find(field_qname); it != field_display_configs_.end()) return *it;
  return sentinel;
}

const ProtoModel::FieldDisplayConfig &ProtoModel::GetFieldDisplay(const std::string &field_qname) const {
  return display_config_.GetFieldDisplay(field_qname);
}

const ProtoModel::MessageDisplayConfig &ProtoModel::DisplayConfig::GetMessageDisplay(
    const std::string &message_qname) const {
  static const ProtoModel::MessageDisplayConfig sentinel(false);
  if (auto it = message_display_configs_.find(message_qname); it != message_display_configs_.end()) return *it;
  return sentinel;
}

const ProtoModel::MessageDisplayConfig &ProtoModel::GetMessageDisplay(const std::string &message_qname) const {
  return display_config_.GetMessageDisplay(message_qname);
}

QModelIndex ProtoModel::parent(const QModelIndex & /*index*/) const { return QModelIndex(); }

Qt::ItemFlags ProtoModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return Qt::NoItemFlags;
  auto flags = QAbstractItemModel::flags(index);
  flags |= Qt::ItemIsEditable;
  return flags;
}

void ProtoModel::SetDisplayConfig(const DisplayConfig &display_config) { display_config_ = display_config; }

QVariant ProtoModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const { return {}; }

QString ProtoModel::GetDisplayName() const {
  QString name = GetFieldDisplay(GetDescriptor()->full_name()).name;
  if (!name.isEmpty()) return name;
  // Require Message to avoid grabbing Nth field name for Nth item in a RepeatedMessageModel...
  if (auto *parent = _parentModel ? _parentModel->TryCastAsMessageModel() : nullptr) {
    if (const auto *fd = parent->GetRowDescriptor(row_in_parent_))
      name = QString::fromStdString(fd->name());
  }
  if (name.isEmpty())
    name = QString::fromStdString(GetDescriptor()->name());
  if (name.isEmpty())
    name = DebugName();
  return name;
}

QIcon ProtoModel::GetDisplayIcon() const { return {}; }

QIcon ProtoModel::GetHeaderIcon() const { return {}; }
