#include "TreeModel.h"

#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "Models/RepeatedImageModel.h"
#include "Models/ResourceModelMap.h"

#include <QCoreApplication>
#include <QItemSelectionModel>

TreeModel::TreeModel(MessageModel *root, QObject *parent, const DisplayConfig &config)
    : QAbstractItemModel(parent), display_config_(config), root_(std::make_unique<Node>(this, nullptr, -1, root, -1)) {}

// =====================================================================================================================
// == Tree Querying ====================================================================================================
// =====================================================================================================================

int TreeModel::columnCount(const QModelIndex &) const { return 1; }

int TreeModel::rowCount(const QModelIndex &parent) const {
  if (parent.column() > 0) return 0;
  return IndexToNode(parent)->children.size();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return {};
  if (!IndexToNode(index)) {
    qDebug() << "WTF?";
  }
  switch (role) {
    case Qt::DisplayRole: return IndexToNode(index)->display_name;
    case Qt::DecorationRole: return IndexToNode(index)->display_icon;
  }
  return {};
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return Qt::NoItemFlags;
  return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  return tr("Name");
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
  if (column) return {};
  return IndexToNode(parent)->index(row);
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
  if (!index.isValid()) return QModelIndex();
  Node *parent = IndexToNode(index)->parent;
  if (!parent || !parent->parent) return {};
  return parent->parent->index(parent->row_in_parent);
}

QModelIndex TreeModel::Node::index(int row) const {
  return backing_tree->createIndex(row, 0, (void*) this);
}

TreeModel::Node *TreeModel::IndexToNode(const QModelIndex &index) const {
  if (index.isValid() && index.internalPointer()) {
    Node *parent = static_cast<Node *>(index.internalPointer());
    return parent->NthChild(index.row());
  } else {
    return root_.get();
  }
}

// =====================================================================================================================
// == Tree Building ====================================================================================================
// =====================================================================================================================

static bool Describes(const FieldPath &field_path, int field_number) {
  return field_path.fields.size() == 1 && field_path.front()->number() == field_number;
}

TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent,
                      MessageModel *model, int row_in_model):
    backing_tree(backing_tree), parent(parent), message_model(model),
    row_in_parent(row_in_parent), row_in_model(row_in_model) {
  backing_tree->MapModel(model, this);
  ComputeDisplayData(backing_tree);
  const FieldMeta &meta = backing_tree->GetMetadata(model->GetDescriptor()->full_name());
  if (meta.custom_editor) return;
  for (int row = 0; row < model->rowCount(); ++row) {
    if (meta.disable_oneof_reassignment && model->IsCulledOneofRow(row)) continue;
    int field_num = model->RowToField(row);
    if (Describes(meta.label_field, field_num)) continue;
    // if (Describes(meta.icon_id_field, field_num)) continue;
    // if (Describes(meta.icon_file_field, field_num)) continue;
    PushChild(model->SubModelForRow(row), row);
  }
  if (meta.is_passthrough && children.size() == 1) {
    std::unique_ptr<Node> child = std::move(children[0]);
    children.clear();
    Absorb(*child);
  }
}

// Construct from Repeated Message Model.
TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent,
                      RepeatedMessageModel *model, int row_in_model):
    backing_tree(backing_tree), parent(parent), repeated_message_model(model),
    row_in_parent(row_in_parent), row_in_model(row_in_model) {
  backing_tree->MapModel(model, this);
  ComputeDisplayData(backing_tree);
  for (int row = 0; row < model->rowCount(); ++row) {
    ProtoModel *submodel = model->GetSubModel<ProtoModel>(row);
    PushChild(submodel, row);
  }
}

// Construct from Repeated Primitive Model (base RepeatedModel).
TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent,
                      RepeatedModel* model, int row_in_model):
    backing_tree(backing_tree), parent(parent), repeated_primitive_model(model),
    row_in_parent(row_in_parent), row_in_model(row_in_model) {
  backing_tree->MapModel(model, this);
  ComputeDisplayData(backing_tree);
  for (int row = 0; row < model->rowCount(); ++row) {
    PushChild(nullptr, row);
  }
}

// Construct as a leaf node.
TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent, LeafData model_row):
    backing_tree(backing_tree), parent(parent), containing_model(model_row.model),
    row_in_parent(row_in_parent), row_in_model(model_row.row) {
  ComputeDisplayData(backing_tree);
}

void TreeModel::MapModel(ProtoModel *model, Node *node) {
  backing_nodes_[model] = node;
}

void TreeModel::Node::PushChild(ProtoModel *model, int source_row) {
  if (!model) {
    // Push the child using our own message_model.
    if (message_model) {
      children.push_back(std::make_unique<Node>(backing_tree, this, children.size(),
                                                LeafData{message_model, source_row}));
    } else if (repeated_primitive_model) {
      children.push_back(std::make_unique<Node>(backing_tree, this, children.size(),
                                                LeafData{repeated_primitive_model, source_row}));
    } else {
      qDebug() << "Attempting to push a TreeMode::Node child field onto a non-message parent Node.";
    }
    return;
  }
  if (auto *subMessage = model->TryCastAsMessageModel()) {
    children.push_back(std::make_unique<Node>(backing_tree, this, children.size(), subMessage, source_row));
    return;
  }
  if (auto *repeatedMessage = model->TryCastAsRepeatedMessageModel()) {
    children.push_back(std::make_unique<Node>(backing_tree, this, children.size(), repeatedMessage, source_row));
    return;
  }
  if (auto *repeatedMessage = model->TryCastAsRepeatedModel()) {
    children.push_back(std::make_unique<Node>(backing_tree, this, children.size(), repeatedMessage, source_row));
    return;
  }
}

void TreeModel::Node::Print(int indent) const {
  for (int i = 0; i < indent; ++i) std::cout << ' ';
  std::cout << '"' << display_name.toStdString() << '"' << std::endl;
  for (auto &nn : children) nn->Print(indent + 2);
}

// =====================================================================================================================
// == Node Mapping =====================================================================================================
// =====================================================================================================================

QModelIndex TreeModel::Node::mapFromSource(const QModelIndex &index) const {
  if (!index.internalPointer()) return {};
  if (index.internalPointer() == message_model) {
    for (auto &child : children) {
      if (child->row_in_model == index.row()) return this->index(child->row_in_parent);
    }
    return {};
  }
  if (index.internalPointer() == repeated_message_model || index.internalPointer() == repeated_primitive_model) {
    if (index.row() > 0 && size_t(index.row()) < children.size()) return this->index(index.row());
    return {};
  }
  if (index.internalPointer() == containing_model) {
    if (!parent || index.row() != row_in_model) {
      qDebug() << "This error case shouldn't be reachable.";
      return {};
    }
    qDebug() << "Cool; how did this happen?";
    return parent->index(row_in_parent);
  }
  qDebug() << "Internal error: attempt to map a TreeModel Node's index from an unassociated model.";
  return {};
}

#if false
QModelIndex TreeModel::mapFromSource(const QModelIndex &sourceIndex) const {
  auto *const model = static_cast<ProtoModel *>(sourceIndex.internalPointer());
  auto mapping = backing_nodes_.find(model);
  if (mapping == backing_nodes_.end()) return {};
  Node *node = *mapping;
  return node->mapFromSource(sourceIndex);
}

QModelIndex TreeModel::mapToSource(const QModelIndex &proxyIndex) const {
  if (!proxyIndex.isValid()) return {};
  Node *n = IndexToNode(proxyIndex);
  if (n->message_model)
    return n->message_model->index(n->NthChild(proxyIndex.row())->row_in_model);
  if (n->repeated_model) {
    if (!n->parent || !n->parent->message_model) {
      qDebug() << "Logic error: somehow, a repeated field doesn't have a message parent.";
      return {};
    }
    return n->parent->message_model->index(n->row_in_model);
  }
  if (n->containing_model)
    return n->containing_model->index(n->row_in_model);
  qDebug() << "Logic error: somehow, a node in a TreeModel doesn't have a message model attached.";
  return {};
}
#endif

// =====================================================================================================================
// == Display Configuration ============================================================================================
// =====================================================================================================================

static const std::string kEmptyString;
QString TreeModel::GetItemName(const Node *item) const { return item ? item->display_name : "<null>"; }
QVariant TreeModel::GetItemIcon(const Node *item) const { return item ? item->display_icon : QVariant(); }
TreeModel::Node *TreeModel::GetNthChild(Node *item, int n) const { return item ? item->NthChild(n) : nullptr; }
int TreeModel::GetChildCount(Node *item) const { return item ? item->children.size() : 0; }
const std::string &TreeModel::GetMessageType(const Node *node) { return node ? node->GetMessageType() : kEmptyString; }

// This can't live exclusively on Node because it requires some metadata about how this model displays fields.
bool TreeModel::SetItemName(Node *item, const QString &name) {
  if (!item) return false;
  return item->SetName(name, GetMetadata(item->GetMessageType()));
}

bool TreeModel::Node::SetName(const QString &name, const FieldMeta &meta) {
  if (!meta.label_field) return false;
  if (message_model) {
    message_model->SetData(meta.label_field, name);
    return true;
  }
  return false;
}
TreeModel::Node *TreeModel::Node::NthChild(int n) const {
  if (n < 0 || (size_t) n > children.size()) {
    qDebug() << "Accessing row " << n << " of a " << children.size() << "-row tree node `" << display_name << "`";
    return nullptr;
  }
  return children[n].get();
}
const std::string &TreeModel::Node::GetMessageType() const {
  if (message_model) return message_model->GetDescriptor()->full_name();
  if (repeated_message_model) return repeated_message_model->GetDescriptor()->full_name();
  return kEmptyString;
}

void TreeModel::DisplayConfig::SetDefaultIcon(const std::string &message, const QString &icon_name) {
  field_meta_[message].icon_name = icon_name;
}
void TreeModel::DisplayConfig::SetMessagePassthrough(const std::string &message) {
  field_meta_[message].is_passthrough = true;
}
void TreeModel::DisplayConfig::DisableOneofReassignment(const std::string &message) {
  field_meta_[message].disable_oneof_reassignment = true;
}
void TreeModel::DisplayConfig::SetMessageIconPathField(const std::string &message, const FieldPath &field_path) {
  field_meta_[message].icon_file_field = field_path;
}
void TreeModel::DisplayConfig::SetMessageLabelField(const std::string &message, const FieldPath &field_path) {
  field_meta_[message].label_field = field_path;
}
void TreeModel::DisplayConfig::SetMessageIconIdField(const std::string &message, const FieldPath &field_path,
                                                     TreeModel::FieldMeta::IconLookupFn icon_lookup_function) {
  field_meta_[message].icon_id_field = field_path;
  field_meta_[message].icon_lookup_function = icon_lookup_function;
}
void TreeModel::DisplayConfig::UseEditorWidget(const std::string &message, EditorLauncher launcher) {
  field_meta_[message].custom_editor = launcher;
}

void TreeModel::Node::ComputeDisplayData(const TreeModel *backing_model) {
  if (message_model) {
    if (parent && parent->message_model) {
      if (const FieldDescriptor *field = parent->message_model->GetDescriptor()->field(row_in_model)) {
        display_name = QString::fromStdString(field->name());
      } else {
        qDebug() << "Message magically spawned a child from nonexisting field" << row_in_model << "of "
                 << parent->message_model->GetDescriptor()->full_name().c_str() << ". It is a miracle.";
      }
    }
    if (display_name.isEmpty()) display_name = QString::fromStdString(message_model->GetDescriptor()->name());
    ComputeRemainingDisplayData(backing_model->GetMetadata(message_model->GetDescriptor()->full_name()));
  } else if (repeated_message_model){
    display_name = QString::fromStdString(repeated_message_model->GetFieldDescriptor()->name());
    ComputeRemainingDisplayData(backing_model->GetMetadata(repeated_message_model->GetDescriptor()->full_name()));
  } else if (repeated_primitive_model){
    display_name = QString::fromStdString(repeated_primitive_model->GetFieldDescriptor()->name());
  } else if (containing_model) {
    if (const FieldDescriptor *field = containing_model->GetRowDescriptor(row_in_model)) {
      display_name = QString::fromStdString(field->name());
      value_field = FieldPath(field);
    } else {
      qDebug() << "Row " << row_in_model << " doesn't exist in model, yet model was constructed for it.";
    }
    if (const QVariant value = containing_model->Data(row_in_model); !value.isNull()) {
      display_name += " = " + value.toString();
    }
  }
}

void TreeModel::Node::ComputeRemainingDisplayData(const FieldMeta &field_meta) {
  if (field_meta.label_field && message_model) {
    display_name = message_model->Data(field_meta.label_field).toString();
    name_field = field_meta.label_field;
  }
  if (field_meta.icon_id_field && message_model) {
    icon_id_field = field_meta.icon_id_field;
    icon_lookup_function = field_meta.icon_lookup_function;
    if (const QVariant value = message_model->Data(icon_id_field); !value.isNull()) {
      if (icon_lookup_function) {
        display_icon = icon_lookup_function(value);
      } else {
        display_icon = ArtManager::GetIcon(value.toString());
      }
    }
  } else if (field_meta.icon_file_field && message_model) {
    icon_path_field = field_meta.icon_file_field;
    if (const QVariant value = message_model->Data(icon_path_field); !value.isNull()) {
      display_icon = ArtManager::GetIcon(value.toString());
    }
  }
  if (display_icon.isNull() && !field_meta.icon_name.isEmpty()) {
    display_icon = ArtManager::GetIcon(field_meta.icon_name);
  }
}

static bool IsUnset(const QIcon &icon) { return icon.isNull(); }
static bool IsUnset(const QString &string) { return string.isEmpty(); }

template <typename T>
std::pair<T, FieldPath> Collapse(T my_value, const FieldPath &my_field, T child_value, const FieldPath &child_field) {
  if (my_field) return {my_value, my_field};
  if (child_field) return {child_value, child_field};
  if (!IsUnset(child_value) || IsUnset(my_value)) return {child_value, child_field};
  return {my_value, my_field};
}

template <typename T>
std::tuple<T, FieldPath, FieldPath> Collapse(
    T my_value, const FieldPath &my_field_1, const FieldPath &my_field_2,
    T child_value, const FieldPath &child_field_1, const FieldPath &child_field_2) {
  if (my_field_1 || my_field_2) return {my_value, my_field_1, my_field_2};
  if (child_field_1 || !child_field_2) return {child_value, child_field_1, child_field_2};
  if (!IsUnset(child_value) || IsUnset(my_value)) return {child_value, child_field_1, child_field_2};
  return {my_value, my_field_1, my_field_2};
}

void TreeModel::Node::Absorb(TreeModel::Node &child) {
  // Our parent and row_in_parent remain unchanged.
  // But otherwise, we become exactly the givene child node.
  if ((message_model = child.message_model)) backing_tree->MapModel(message_model, this);
  if ((repeated_message_model = child.repeated_message_model)) backing_tree->MapModel(repeated_message_model, this);
  if ((repeated_primitive_model = child.repeated_primitive_model)) backing_tree->MapModel(repeated_primitive_model, this);
  if ((containing_model = child.containing_model)) backing_tree->MapModel(containing_model, this);
  row_in_model = child.row_in_model;
  std::tie(display_name, name_field) = Collapse(display_name, name_field, child.display_name, child.name_field);
  std::tie(display_icon, icon_id_field, icon_path_field) =
      Collapse(display_icon, icon_id_field, icon_path_field,
               child.display_icon, child.icon_id_field, child.icon_path_field);
  value_field = child.value_field;
  children.swap(child.children);

  // Our parent doesn't change... but our children's parents sure do.
  for (auto &child : children) child->parent = this;
}

// =====================================================================================================================
// == Data Management Helpers ==========================================================================================
// =====================================================================================================================

QModelIndex TreeModel::insert(const QModelIndex &parent, int row, const Message &message) {
  if (Node *parentNode = IndexToNode(parent)) {
    // We will respond to the message emitted by this model change and update our own model, then.
    return parentNode->insert(message, row);
  }
  return QModelIndex();
}

QModelIndex TreeModel::addNode(const Message &child, const QModelIndex &parent) {
  int pos = 0;
  auto insertParent = parent;
  if (parent.isValid()) {
    Node *parentNode = IndexToNode(parent);
    if (parentNode->IsRepeated()) {
      pos = parentNode->children.size();
    } else {
      insertParent = parent.parent();
      pos = parent.row();
    }
  } else {
    insertParent = QModelIndex();
    pos = root_->children.size();
  }

  return insert(insertParent, pos, child);
}

QModelIndex TreeModel::duplicateNode(const QModelIndex &index) {
  /*Node *node = IndexToNode(index);
  if (node->repeated_model) {
    return node->mapFromSource(node->repeated_model->duplicate(mapToSource(index)));
  }*/
  qDebug() << rowCount() << " rows";
  return QModelIndex();
}

void TreeModel::removeNode(const QModelIndex &index) {/*
  if (!index.isValid()) return;
  auto *node = static_cast<Message *>(index.internalPointer());
  if (!node) return;
  if (node->has_folder()) {
    for (int i = node->child_size(); i > 0; --i) {
      removeNode(this->index(i - 1, 0, index));
    }
  }
  Message *parent = parents[node];
  int pos = 0;
  for (; pos < parent->child_size(); ++pos)
    if (parent->mutable_child(pos) == node) break;
  if (pos == parent->child_size()) return;  // already removed?
  emit beginRemoveRows(index.parent(), pos, pos);
  resourceMap->RemoveResource(node->type_case(), QString::fromStdString(node->name()));
  parent->mutable_child()->DeleteSubrange(pos, 1);
  emit endRemoveRows();*/
}

void TreeModel::sortByName(const QModelIndex &index) {/*
  if (!index.isValid()) return;
  auto *node = static_cast<Message *>(index.internalPointer());
  if (!node) return;
  auto *child_field = node->mutable_child();
  std::sort(child_field->begin(), child_field->end(),
            [](const Message &a, const Message &b) { return a.name() < b.name(); });
  emit dataChanged(this->index(0, 0, index), this->index(node->child_size(), 0, index));*/
}


void TreeModel::triggerNodeEdit(const QModelIndex &index, QAbstractItemView *view) {
  if (!index.isValid() || !index.internalPointer()) return;
  Node *node = IndexToNode(index);
  if (node) {
    const auto &meta = GetMetadata(GetMessageType(node));
    if (meta.custom_editor) {
      if (ProtoModel *model = node->BackingModel()) {
        if (auto *mmodel = model->As<MessageModel>()) {
          meta.custom_editor(mmodel);
          return;
        }
      }
    }
  }
  // select the new node so that it gets "revealed" and its parent is expanded
  view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  // start editing the name of the resource in the tree for convenience
  view->edit(index);
}

QModelIndex TreeModel::Node::insert(const Message &message, int row) {
  if (repeated_message_model) return mapFromSource(repeated_message_model->insert(message, row));
  return QModelIndex();
}
bool TreeModel::Node::IsRepeated() const {
  return repeated_message_model || repeated_primitive_model;
}
ProtoModel *TreeModel::Node::BackingModel() const {
  if (repeated_message_model) return repeated_message_model;
  if (repeated_primitive_model) return repeated_primitive_model;
  if (message_model) return message_model;
  if (containing_model) return containing_model;
  qDebug() << "Tree node has no backing model...?";
  return nullptr;
}

const TreeModel::FieldMeta &TreeModel::DisplayConfig::GetMetadata(const std::string &message_qname) const {
  static const FieldMeta sentinel;
  if (auto it = field_meta_.find(message_qname); it != field_meta_.end()) return *it;
  return sentinel;
}

const TreeModel::FieldMeta &TreeModel::GetMetadata(const std::string &message_qname) const {
  return display_config_.GetMetadata(message_qname);
}
