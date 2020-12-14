#include "TreeModel.h"

#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "Models/RepeatedImageModel.h"
#include "Models/ResourceModelMap.h"

#include <QCoreApplication>
#include <QItemSelectionModel>

TreeModel::TreeModel(MessageModel *root, QObject *parent)
    : QAbstractItemModel(parent), root_(std::make_unique<Node>(this, nullptr, -1, root)) {}

// =====================================================================================================================
// == Tree Querying ====================================================================================================
// =====================================================================================================================

int TreeModel::columnCount(const QModelIndex & /*parent*/) const { return 1; }

int TreeModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid();//GetChildCount(IndexToNode(parent));
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || role != Qt::DisplayRole) return {};
  return "boobies!!!";
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
  return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  return tr("Name");
}

TreeModel::Node *TreeModel::IndexToNode(const QModelIndex &index) const {
  if (index.isValid() && index.internalPointer()) {
    return static_cast<Node *>(index.internalPointer());
  } else {
    return root_.get();
  }
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
  return createIndex(row, column);
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
  if (!index.isValid()) return QModelIndex();
  return createIndex(index.row(), index.column());
}

// =====================================================================================================================
// == Tree Building ====================================================================================================
// =====================================================================================================================

TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent, MessageModel *model):
    backing_tree(backing_tree), parent(parent), message_model(model), row_in_parent(row_in_parent), row_in_model(-1) {
  backing_tree->MapModel(model, this);
  displayName = "boobs";
  const auto &subModels = model->SubModels();
  for (auto it = subModels.begin(); it != subModels.end(); ++it) {
    int row = it.key();
    ProtoModel *submodel = it.value();
    PushChild(submodel, row);
  }
}

TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent, RepeatedMessageModel *model):
    backing_tree(backing_tree), parent(parent), repeated_model(model), row_in_parent(row_in_parent), row_in_model(-1) {
  backing_tree->MapModel(model, this);
  displayName = "bewbs";
  for (int row = 0; row < model->rowCount(); ++row) {
    ProtoModel *submodel = model->GetSubModel<ProtoModel>(row);
    PushChild(submodel, row);
  }
}

TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent, MessageModel *model, int row_in_model):
    backing_tree(backing_tree), parent(parent), containing_model(model), row_in_parent(row_in_parent),
    row_in_model(row_in_model) {
  displayName = "b00bies";
}

void TreeModel::MapModel(ProtoModel *model, Node *node) {
  backing_nodes_[model] = node;
}

void TreeModel::Node::PushChild(ProtoModel *model, int source_row) {
  if (auto *subMessage = model->AsMessageModel()) {
    children.push_back(std::make_unique<Node>(backing_tree, this, children.size(), subMessage));
    return;
  }
  if (auto *repeatedMessage = model->AsRepeatedMessageModel()) {
    children.push_back(std::make_unique<Node>(backing_tree, this, children.size(), repeatedMessage));
    return;
  }
  // Push the child using our own message_model.
  if (message_model) {
    children.push_back(std::make_unique<Node>(backing_tree, this, children.size(), message_model, source_row));
  } else {
    qDebug() << "Attempting to push a TreeMode::Node child field onto a non-message parent Node.";
  }
}

void TreeModel::Node::Print(int indent) const {
  for (int i = 0; i < indent; ++i) std::cout << ' ';
  std::cout << '"' << displayName.toStdString() << '"' << std::endl;
  for (auto &nn : children) nn->Print(indent + 2);
}

// =====================================================================================================================
// == Node Mapping =====================================================================================================
// =====================================================================================================================

QModelIndex TreeModel::Node::index(int row) const {
  return backing_tree->createIndex(row, 0, (void*) this);
}

QModelIndex TreeModel::Node::mapFromSource(const QModelIndex &index) const {
  if (!index.internalPointer()) return {};
  if (index.internalPointer() == message_model) {
    for (auto &child : children) {
      if (child->row_in_model == index.row()) return this->index(child->row_in_parent);
    }
    return {};
  }
  if (index.internalPointer() == repeated_model) {
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
QString TreeModel::GetItemName(const Node *item) const { return item ? item->displayName : "<null>"; }
QVariant TreeModel::GetItemIcon(const Node *item) const { return item ? item->displayIcon : QVariant(); }
TreeModel::Node *TreeModel::GetNthChild(Node *item, int n) const { return item ? item->NthChild(n) : nullptr; }
int TreeModel::GetChildCount(Node *item) const { return item ? item->children.size() : 0; }
const std::string &TreeModel::GetMessageType(const Node *node) { return node ? node->GetMessageType() : kEmptyString; }

// This can't live exclusively on Node because it requires some metadata about how this model displays fields.
bool TreeModel::SetItemName(Node *item, const QString &name) {
  if (!item) return false;
  auto f = field_meta_.find(item->GetMessageType());
  if (f == field_meta_.end()) return false;
  return item->SetName(name, &*f);
}

bool TreeModel::Node::SetName(const QString &name, const FieldMeta *meta) {
  if (!meta || !meta->label_field) return false;
  if (message_model) {
    message_model->SetData(meta->label_field, name);
    return true;
  }
  return false;
}
TreeModel::Node *TreeModel::Node::NthChild(int n) const {
  if (n < 0 || (size_t) n > children.size()) return nullptr;
  return children[n].get();
}
const std::string &TreeModel::Node::GetMessageType() const {
  if (message_model) return message_model->GetDescriptor()->full_name();
  if (repeated_model) return repeated_model->GetDescriptor()->full_name();
  return kEmptyString;
}

void TreeModel::SetDefaultIcon(const std::string &message, const QString &icon_name) {
  field_meta_[message].icon_name = icon_name;
}
void TreeModel::SetMessagePassthrough(const std::string &message) {}
void TreeModel::SetMessageIconPathField(const std::string &message, int field_number) {}
void TreeModel::SetMessageLabelField(const std::string &message, int field_number) {}
void TreeModel::SetMessageIconNameField(const std::string &message, int field_number, const QString &pattern) {}
void TreeModel::UseEditorWidget(const std::string &message, EditorFactory factory) {}

// =====================================================================================================================
// == Data Management Helpers ==========================================================================================
// =====================================================================================================================

QModelIndex TreeModel::insert(const QModelIndex &parent, int row, const Message &message) {
  auto insertIndex = parent;
  if (!parent.isValid()) insertIndex = QModelIndex();
  auto *parentNode = IndexToNode(insertIndex);
  if (!parentNode) {
    insertIndex = QModelIndex();
    parentNode = root_.get();
  }
  if (!parentNode->repeated_model
      || parentNode->repeated_model->MessageName() != message.GetDescriptor()->full_name()) {
    return QModelIndex();
  }

  // We will respond to the message emitted by this model change and update our own model, then.
  parentNode->repeated_model->insert(message, row);

  return this->index(row, 0, insertIndex);
}

QModelIndex TreeModel::addNode(const Message &child, const QModelIndex &parent) {
  int pos = 0;
  auto insertParent = parent;
  if (parent.isValid()) {
    Node *parentNode = IndexToNode(parent);
    if (parentNode->repeated_model) {
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
  auto *node = IndexToNode(index);
  auto meta = field_meta_.find(GetMessageType(node));
  if (meta != field_meta_.end()) {
    if (meta->custom_editor) {
      meta->custom_editor(node->message_model);
      return;
    }
  }
  // select the new node so that it gets "revealed" and its parent is expanded
  view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  // start editing the name of the resource in the tree for convenience
  view->edit(index);
}
