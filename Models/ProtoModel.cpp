#include "ProtoModel.h"
#include "MessageModel.h"
#include "RepeatedMessageModel.h"
#include "RepeatedModel.h"

#include "Components/Logger.h"

ProtoModel::ProtoModel(QObject *parent, std::string name, const Descriptor *descriptor) :
  ProtoModel(static_cast<ProtoModel *>(nullptr), name, descriptor) {
  QObject::setParent(parent);
}

ProtoModel::ProtoModel(ProtoModel *parent, std::string name, const Descriptor *descriptor)
    : QAbstractItemModel(parent), _dirty(false), _parentModel(parent),
      _debug_path((parent ? parent->_debug_path + "." : "") + name), descriptor_(descriptor) {
  connect(this, &ProtoModel::DataChanged, this,
          [this](const QModelIndex &topLeft, const QModelIndex &bottomRight,
                 const QVariant & /*oldValue*/ = QVariant(0), const QVector<int> &roles = QVector<int>()) {
            emit QAbstractItemModel::dataChanged(topLeft, bottomRight, roles);
          });
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

void ProtoModel::DisplayConfig::SetDefaultIcon(const std::string &message, const QString &icon_name) {
  field_display_configs_[message].icon_name = icon_name;
}
void ProtoModel::DisplayConfig::SetMessageIconPathField(const std::string &message, const FieldPath &field_path) {
  message_display_configs_[message].icon_field = field_path;
}
void ProtoModel::DisplayConfig::SetMessageLabelField(const std::string &message, const FieldPath &field_path) {
  message_display_configs_[message].label_field = field_path;
}
void ProtoModel::DisplayConfig::SetMessageIconIdField(const std::string &message, const FieldPath &field_path,
                                                     FieldDisplayConfig::IconLookupFn icon_lookup_function) {
  field_display_configs_[message].icon_file_field = field_path;
  field_display_configs_[message].icon_lookup_function = icon_lookup_function;
}

const ProtoModel::FieldDisplayConfig &ProtoModel::DisplayConfig::GetFieldDisplay(const std::string &message_qname) const {
  static const ProtoModel::FieldDisplayConfig sentinel;
  if (auto it = field_display_configs_.find(message_qname); it != field_display_configs_.end()) return *it;
  return sentinel;
}

const ProtoModel::FieldDisplayConfig & ProtoModel::GetFieldDisplay(const std::string &message_qname) const {
  return display_config_.GetFieldDisplay(message_qname);
}

const ProtoModel::MessageDisplayConfig &ProtoModel::DisplayConfig::GetMessageDisplay(const std::string &message_qname) const {
  static const ProtoModel::MessageDisplayConfig sentinel;
  if (auto it = message_display_configs_.find(message_qname); it != message_display_configs_.end()) return *it;
  return sentinel;
}

const ProtoModel::MessageDisplayConfig & ProtoModel::GetMessageDisplay(const std::string &message_qname) const {
  return display_config_.GetMessageDisplay(message_qname);
}

QModelIndex ProtoModel::parent(const QModelIndex & /*index*/) const { return QModelIndex(); }

Qt::ItemFlags ProtoModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return Qt::NoItemFlags;
  auto flags = QAbstractItemModel::flags(index);
  flags |= Qt::ItemIsEditable;
  return flags;
}

void ProtoModel::SetDisplayConfig(const DisplayConfig &display_config) {
  display_config_ = display_config;
}

bool ProtoModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) {
  if (orientation == Qt::Horizontal) {
    _horizontalHeaderData[section][static_cast<Qt::ItemDataRole>(role)] = value;
    emit headerDataChanged(Qt::Horizontal, section, section);
    return true;
  } else if (orientation == Qt::Vertical) {
    _verticalHeaderData[section][static_cast<Qt::ItemDataRole>(role)] = value;
    emit headerDataChanged(Qt::Vertical, section, section);
    return true;
  }
  return QAbstractItemModel::setHeaderData(section, orientation, value, role);
}

static QVariant getHeaderData(const QHash<int,QHash<Qt::ItemDataRole,QVariant>>& map, int section, int role) {
  auto sit = map.find(section);
  if (sit == map.end()) return QVariant();
  auto sectionMap = *sit;
  auto it = sectionMap.find(static_cast<Qt::ItemDataRole>(role));
  if (it == sectionMap.end()) return QVariant();
  return *it;
}

QVariant ProtoModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal) {
    return getHeaderData(_horizontalHeaderData, section, role);
  } else if (orientation == Qt::Vertical) {
    return getHeaderData(_verticalHeaderData, section, role);
  }
  return QVariant();
}
