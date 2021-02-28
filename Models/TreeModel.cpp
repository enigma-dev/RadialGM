#include "TreeModel.h"

#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "Models/ResourceModelMap.h"

#include <QCoreApplication>
#include <QItemSelectionModel>
#include <QMimeData>

TreeModel::TreeModel(MessageModel *root, QObject *parent, const DisplayConfig &config)
    : QAbstractItemModel(parent), mime_types_(GetMimeTypes(root->GetDescriptor())),
      display_config_(config), root_(std::make_unique<Node>(this, nullptr, -1, root, -1)) {}

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
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);
  if (index.isValid()) flags |= Qt::ItemIsDragEnabled;
  flags |= Qt::ItemIsDropEnabled;
  return flags;
}

Qt::DropActions TreeModel::supportedDropActions() const {
  return Qt::MoveAction | Qt::CopyAction;
}

QStringList TreeModel::mimeTypes() const {
  return mime_types_;
}

QMimeData *TreeModel::mimeData(const QModelIndexList &indexes) const {
  QModelIndexList sortedIndexes = indexes;
  std::sort(sortedIndexes.begin(), sortedIndexes.end(),
            [](QModelIndex& a, QModelIndex& b) { return a.row() < b.row(); });

  QMimeData* mimeData = new QMimeData();
  QByteArray encodedData;
  QDataStream stream(&encodedData, QIODevice::WriteOnly);

  foreach (const QModelIndex& index, sortedIndexes) {
    if (index.isValid()) {
      QString text = data(index, Qt::UserRole).toString();
      stream << text;
      stream << index.row();
    }
  }

  mimeData->setData(mimeTypes()[0], encodedData);
  return mimeData;
}

bool TreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                             const QModelIndex &parent) {
  return false;
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
    backing_tree(backing_tree), parent(parent), backing_model(model),
    row_in_parent(row_in_parent), row_in_model(row_in_model) {
  backing_tree->MapModel(model, this);
  ComputeDisplayData();
  const auto &tree_meta = backing_tree->GetTreeDisplay(model->GetDescriptor()->full_name());
  const auto &msg_meta = BackingModel()->GetMessageDisplay(model->GetDescriptor()->full_name());
  if (tree_meta.custom_editor) return;
  for (int row = 0; row < model->rowCount(); ++row) {
    if (tree_meta.disable_oneof_reassignment && model->IsCulledOneofRow(row)) continue;
    int field_num = model->RowToField(row);
    if (Describes(msg_meta.label_field, field_num)) continue;
    // if (Describes(meta.icon_id_field, field_num)) continue;
    // if (Describes(meta.icon_file_field, field_num)) continue;
    PushChild(model->SubModelForRow(row), row);
  }
  if (tree_meta.is_passthrough && children.size() == 1) {
    std::unique_ptr<Node> child = std::move(children[0]);
    children.clear();
    Absorb(*child);
  }
}

// Construct from Repeated Message Model.
TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent,
                      RepeatedMessageModel *model, int row_in_model):
    backing_tree(backing_tree), parent(parent), backing_model(model),
    row_in_parent(row_in_parent), row_in_model(row_in_model) {
  backing_tree->MapModel(model, this);
  ComputeDisplayData();
  for (int row = 0; row < model->rowCount(); ++row) {
    ProtoModel *submodel = model->GetSubModel<ProtoModel>(row);
    PushChild(submodel, row);
  }
}

// Construct from Repeated Primitive Model (base RepeatedModel).
TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent,
                      RepeatedModel* model, int row_in_model):
    backing_tree(backing_tree), parent(parent), backing_model(model),
    row_in_parent(row_in_parent), row_in_model(row_in_model) {
  backing_tree->MapModel(model, this);
  ComputeDisplayData();
  for (int row = 0; row < model->rowCount(); ++row) {
    PushChild(model->GetSubModel(row), row);
  }
}

// Construct as a leaf node.
TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent,
                      PrimitiveModel *model, int row_in_model):
    backing_tree(backing_tree), parent(parent), backing_model(model),
    row_in_parent(row_in_parent), row_in_model(row_in_model) {
  ComputeDisplayData();
}

void TreeModel::MapModel(ProtoModel *model, Node *node) {
  backing_nodes_[model] = node;
}

void TreeModel::Node::PushChild(ProtoModel *model, int source_row) {
  if (!model) {
    qDebug() << "Null model passed to TreeNode::PushChild()...";
    return;
  }
  if (auto *sub_message = model->TryCastAsMessageModel()) {
    children.push_back(std::make_unique<Node>(backing_tree, this, children.size(), sub_message, source_row));
  } else if (auto *repeated_message = model->TryCastAsRepeatedMessageModel()) {
    children.push_back(std::make_unique<Node>(backing_tree, this, children.size(), repeated_message, source_row));
  } else if (auto *repeated_message = model->TryCastAsRepeatedModel()) {
    children.push_back(std::make_unique<Node>(backing_tree, this, children.size(), repeated_message, source_row));
  } else if (auto *primitive_message = model->TryCastAsPrimitiveModel()) {
    children.push_back(std::make_unique<Node>(backing_tree, this, children.size(), primitive_message, source_row));
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
  // For messages, translate field index() to child offset in children vector.
  if (auto *message_model = backing_model->TryCastAsMessageModel(); index.internalPointer() == message_model) {
    for (auto &child : children) {
      if (child->row_in_model == index.row()) return this->index(child->row_in_parent);
    }
    return {};
  }
  // All other index values should be 1:1.
  if (index.row() > 0 && size_t(index.row()) < children.size()) return this->index(index.row());
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
// bool TreeModel::SetItemName(Node *item, const QString &name) {
//   if (!item) return false;
//   return item->SetName(name, item->BackingModel()->GetMessageDisplay(item->GetMessageType()));
// }

TreeModel::Node *TreeModel::Node::NthChild(int n) const {
  if (n < 0 || (size_t) n > children.size()) {
    qDebug() << "Accessing row " << n << " of a " << children.size() << "-row tree node `" << display_name << "`";
    return nullptr;
  }
  return children[n].get();
}
const std::string &TreeModel::Node::GetMessageType() const {
  return backing_model->GetDescriptor()->full_name();
}

void TreeModel::DisplayConfig::SetMessagePassthrough(const std::string &message) {
  tree_display_configs_[message].is_passthrough = true;
}
void TreeModel::DisplayConfig::DisableOneofReassignment(const std::string &message) {
  tree_display_configs_[message].disable_oneof_reassignment = true;
}
void TreeModel::DisplayConfig::UseEditorWidget(const std::string &message, EditorLauncher launcher) {
  tree_display_configs_[message].custom_editor = launcher;
}

void TreeModel::Node::ComputeDisplayData() {
  display_name = backing_model->GetDisplayName();
  /*if (auto *primitive_model = backing_model->TryCastAsPrimitiveModel()) {
    if (const QVariant value = primitive_model->GetDirect(); !value.isNull()) {
      display_name += " = " + value.toString();
    }
  }*/
  display_icon = backing_model->GetDisplayIcon();
}

// template <typename T>
// std::pair<T, FieldPath> Collapse(T my_value, const FieldPath &my_field, T child_value, const FieldPath &child_field) {
//   if (my_field) return {my_value, my_field};
//   if (child_field) return {child_value, child_field};
//   if (!IsUnset(child_value) || IsUnset(my_value)) return {child_value, child_field};
//   return {my_value, my_field};
// }
//
// template <typename T>
// std::tuple<T, FieldPath, FieldPath> Collapse(
//     T my_value, const FieldPath &my_field_1, const FieldPath &my_field_2,
//     T child_value, const FieldPath &child_field_1, const FieldPath &child_field_2) {
//   if (my_field_1 || my_field_2) return {my_value, my_field_1, my_field_2};
//   if (child_field_1 || !child_field_2) return {child_value, child_field_1, child_field_2};
//   if (!IsUnset(child_value) || IsUnset(my_value)) return {child_value, child_field_1, child_field_2};
//   return {my_value, my_field_1, my_field_2};
// }

void TreeModel::Node::Absorb(TreeModel::Node &child) {
  // Our parent and row_in_parent remain unchanged.
  // But otherwise, we become exactly the givene child node.
  backing_model = child.backing_model;
  backing_tree->MapModel(backing_model, this);
  row_in_model = child.row_in_model;
  // std::tie(display_name, name_field) = Collapse(display_name, child.display_name, child.name_field);
  // std::tie(display_icon, icon_id_field, icon_path_field) =
  //     Collapse(display_icon, icon_id_field, icon_path_field,
  //              child.display_icon, child.icon_id_field, child.icon_path_field);
  // value_field = child.value_field;
  //if (!child.display_name.isEmpty()) display_name = child.display_name;
  if (!child.display_icon.isNull()) display_icon = child.display_icon;
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

QModelIndex TreeModel::duplicateNode(const QModelIndex &/*index*/) {
  /*Node *node = IndexToNode(index);
  if (node->repeated_model) {
    return node->mapFromSource(node->repeated_model->duplicate(mapToSource(index)));
  }*/
  qDebug() << rowCount() << " rows";
  return QModelIndex();
}

void TreeModel::removeNode(const QModelIndex &/*index*/) {/*
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

void TreeModel::sortByName(const QModelIndex &/*index*/) {/*
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
    const auto &meta = GetTreeDisplay(GetMessageType(node));
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
  if (auto *const repeated_message_model = backing_model->TryCastAsRepeatedMessageModel())
    return mapFromSource(repeated_message_model->insert(message, row));
  return QModelIndex();
}
bool TreeModel::Node::IsRepeated() const {
  return backing_model->TryCastAsRepeatedModel();
}
ProtoModel *TreeModel::Node::BackingModel() const {
  return backing_model;
}

const TreeModel::TreeNodeDisplayConfig &TreeModel::DisplayConfig::GetTreeDisplay(const std::string &message_qname) const {
  static const TreeModel::TreeNodeDisplayConfig sentinel;
  if (auto it = tree_display_configs_.find(message_qname); it != tree_display_configs_.end()) return *it;
  return sentinel;
}

const TreeModel::TreeNodeDisplayConfig &TreeModel::GetTreeDisplay(const std::string &message_qname) const {
  return display_config_.GetTreeDisplay(message_qname);
}
