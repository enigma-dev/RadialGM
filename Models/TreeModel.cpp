#include "TreeModel.h"

#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "Models/RepeatedImageModel.h"
#include "Models/ResourceModelMap.h"

#include <QCoreApplication>
#include <QItemSelectionModel>
#include <QMimeData>

TreeModel::TreeModel(MessageModel *root, QObject *parent)
    : QAbstractProxyModel(parent), root_(std::make_unique<Node>(this, root)) {}

int TreeModel::columnCount(const QModelIndex & /*parent*/) const { return 1; }

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  R_EXPECT(index.isValid(), false) << "Supplied index was invalid:" << index;
  if (role != Qt::EditRole) return false;

  Node *item = static_cast<Node *>(index.internalPointer());
  const QString oldName = GetItemName(item);
  const QString newName = value.toString();
  if (oldName == newName) return true;
  if (!SetItemName(item, newName)) return false;
  emit ItemRenamed(item, oldName, value.toString());
  emit dataChanged(index, index);
  return true;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
  R_EXPECT(index.isValid(), QVariant()) << "Supplied index was invalid:" << index;

  Node *item = static_cast<Node *>(index.internalPointer());
  if (role == Qt::DecorationRole) {
    return GetItemIcon(item);
  } else if (role == Qt::EditRole || role == Qt::DisplayRole) {
    return item->displayName;
  }

  return QVariant();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);

  if (index.isValid()) {
    auto *node = static_cast<Node *>(index.internalPointer());
    if (node->repeated_model) flags |= Qt::ItemIsDropEnabled;
    return Qt::ItemIsDragEnabled | Qt::ItemIsEditable | flags;
  } else
    return Qt::ItemIsDropEnabled | flags;
}

QVariant TreeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  return tr("Name");
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent)) return QModelIndex();

  Node *parentItem;

  if (parent.isValid()) {
    parentItem = static_cast<Node *>(parent.internalPointer());
  } else {
    parentItem = root_.get();
  }

  Node *childItem = GetNthChild(parentItem, row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
  if (!index.isValid()) return QModelIndex();

  Node *childItem = static_cast<Node *>(index.internalPointer());
  Node *parentItem = childItem->parent;

  if (!parentItem || parentItem == root_.get()) return QModelIndex();
  return createIndex(parentItem->row_in_parent, 0, parentItem);
}

TreeModel::Node *TreeModel::IndexToNode(const QModelIndex &index) const {
  if (index.isValid() && index.internalPointer()) {
    return static_cast<Node *>(index.internalPointer());
  } else {
    return root_.get();
  }
}

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


int TreeModel::rowCount(const QModelIndex &parent) const {
  if (parent.column() > 0) return 0;
  return GetChildCount(IndexToNode(parent));
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

TreeModel::Node::Node(TreeModel *backing_tree, MessageModel *model): backing_tree(backing_tree) {
  const auto &subModels = model->SubModels();
  for (auto it = subModels.begin(); it != subModels.end(); ++it) {
    int row = it.key();
    ProtoModel *model = it.value();
    if (auto *subMessage = model->AsMessageModel()) {
      children.push_back(std::make_unique<Node>(backing_tree, subMessage));
      continue;
    }
    if (auto *repeatedMessage = model->AsRepeatedMessageModel()) {
      children.push_back(std::make_unique<Node>(backing_tree, repeatedMessage));
      continue;
    }
    if (auto *subMessage = model->AsMessageModel()) {
      children.push_back(std::make_unique<Node>(backing_tree, subMessage, row));
      continue;
    }
  }
}

QModelIndex TreeModel::Node::mapFromSource(const QModelIndex &index) const {
  if (!index.internalPointer()) return {};
  if (index.internalPointer() == message_model) {
    for (auto &child : children) {
      if (child->row_in_model == index.row()) return this->index(child->row_in_parent);
    }
  }
  qDebug() << "Internal error: attempt to map a TreeModel Node's index from an unassociated model.";
  return {};
}

QModelIndex TreeModel::Node::index(int row) const {
  return QModelIndex(row, 0, (void*) this, backing_tree);
}

Qt::DropActions TreeModel::supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

QStringList TreeModel::mimeTypes() const { return QStringList(treeNodeMime()); }

QMimeData *TreeModel::mimeData(const QModelIndexList &indexes) const {
  QMimeData *mimeData = new QMimeData();
  QByteArray data;

  QDataStream stream(&data, QIODevice::WriteOnly);
  QList<QModelIndex> nodes;

  for (const QModelIndex &index : indexes) {
    if (!index.isValid() || nodes.contains(index)) continue;
    nodes << index;
  }

  // rows are moved starting with the lowest so we can create
  // unique names in the order of insertion
  std::sort(nodes.begin(), nodes.end(), std::less<QModelIndex>());

  stream << QCoreApplication::applicationPid();
  stream << nodes.count();
  for (const QModelIndex &index : nodes) {
    Node *node = static_cast<Node *>(index.internalPointer());
    stream << reinterpret_cast<qlonglong>(node) << index.row();
  }
  mimeData->setData(treeNodeMime(), data);
  return mimeData;
}

template <typename T>
static void RepeatedFieldInsert(RepeatedPtrField<T> *field, T *newItem, int index) {
  field->AddAllocated(newItem);
  for (int j = field->size() - 1; j > index; --j) {
    field->SwapElements(j, j - 1);
  }
}

QModelIndex TreeModel::mapFromSource(const QModelIndex &sourceIndex) const {
  return {};
}
QModelIndex TreeModel::mapToSource(const QModelIndex &proxyIndex) const {
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

bool TreeModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int /*column*/,
                             const QModelIndex &parent) {
  if (action != Qt::MoveAction && action != Qt::CopyAction) return false;
  // ensure the data is in the format we expect
  if (!mimeData->hasFormat(treeNodeMime())) return false;
  QByteArray data = mimeData->data(treeNodeMime());
  QDataStream stream(&data, QIODevice::ReadOnly);

  qint64 senderPid;
  stream >> senderPid;
  // ensure the data is coming from the same process since mime is pointer based
  if (senderPid != QCoreApplication::applicationPid()) return false;

  Node *parentNode = static_cast<Node *>(parent.internalPointer());
  if (!parentNode) parentNode = root_.get();
  int count;
  stream >> count;
  if (count <= 0) return false;
  if (row == -1) row = rowCount(parent);
  QHash<Node *, unsigned> removedCount;

  for (int i = 0; i < count; ++i) {
    qlonglong nodePtr;
    stream >> nodePtr;
    int itemRow;
    stream >> itemRow;
    Node *node = reinterpret_cast<Node *>(nodePtr);

    if (action != Qt::CopyAction) {
      auto *oldParent = node->parent;

      // offset the row we are removing by the number of
      // rows already removed from the same parent
      if (parentNode != oldParent || row > itemRow) {
        itemRow -= removedCount[oldParent];
      }

      auto index = this->createIndex(itemRow, 0, node);
      bool canDo = beginMoveRows(index.parent(), itemRow, itemRow, parent, row);
      if (!canDo) continue;

      // count this row as having been moved from this parent
      if (parentNode != oldParent || row > itemRow) removedCount[oldParent]++;

      // if moving the node within the same parent we need to adjust the row
      // since its own removal will affect the row we reinsert it at
      if (parentNode == oldParent && row > itemRow) --row;
/*
      auto oldRepeated = oldParent->mutable_child();
      oldRepeated->ExtractSubrange(itemRow, 1, nullptr);
      parentNode->model->insert(node->model, row);
      parents[node] = parentNode;
      endMoveRows();
      ++row;

      emit ResourceMoved(node, oldParent);
    } else {
      if (node->folder()) continue;
      node = duplicateNode(*node);
      insert(parent, row++, node);*/
    }
  }

  return true;
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
  Node *node = IndexToNode(index);
  if (node->repeated_model) {
    return node->mapFromSource(node->repeated_model->duplicate(mapToSource(index)));
  }
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
