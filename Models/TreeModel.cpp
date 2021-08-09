#include "TreeModel.h"
#include "MainWindow.h"

#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "Models/ResourceModelMap.h"

#include <QCoreApplication>
#include <QItemSelectionModel>
#include <QMimeData>
#include <QStack>

static QSet<const QModelIndex> GroupNodes(const QSet<const QModelIndex> &nodes) {
  QSet <const QModelIndex> ret;
  for (const auto& n : nodes) {
    QModelIndex t = n;
    bool add = true;
    while (t.parent().isValid()) {
      if (nodes.contains(t.parent())) {
        add = false;
        break;
      }
      t = t.parent();
    }
    if (add) ret.insert(n);
  }
  return ret;
}

TreeModel::TreeModel(MessageModel *root, QObject *parent, const DisplayConfig &config)
    : QAbstractItemModel(parent),
      mime_types_(GetMimeTypes(root->GetDescriptor())),
      display_config_(config),
      root_(std::make_shared<Node>(this, nullptr, -1, root, -1)),
      root_model_(root) {
  RebuildModelMapping();
  connect(root, &MessageModel::modelReset, this, &TreeModel::DataBlownAway);
}

void TreeModel::DataBlownAway() {
  emit TreeChanged(root_model_);
  beginResetModel();
  root_->RebuildFromModel(root_model_, nullptr, 0);
  RebuildModelMapping();
  endResetModel();
}

void TreeModel::RebuildModelMapping() {
  // Drop references to orphans.
  QHash<ProtoModel *, std::shared_ptr<Node>> temp;
  temp.swap(backing_nodes_);
  root_->AddSelfToMap(root_);
}

void TreeModel::Node::AddSelfToMap(const std::shared_ptr<Node> &self) {
  backing_tree->live_nodes.insert(this);
  if (passthrough_model) {
    backing_tree->backing_nodes_.insert(passthrough_model, self);
    if (passthrough_node) {
      passthrough_node->AddSelfToMap(passthrough_node);
    } else {
      qDebug() << "Child node has passthrough model but not passthrough node.";
    }
  } else {
    backing_tree->backing_nodes_.insert(backing_model, self);
  }

  for (const auto &child : children) child->AddSelfToMap(child);
}

// =====================================================================================================================
// == Tree Querying ====================================================================================================
// =====================================================================================================================

int TreeModel::columnCount(const QModelIndex &) const { return 1; }

int TreeModel::rowCount(const QModelIndex &parent) const {
  if (parent.column() > 0) return 0;
  Node *node = IndexToNode(parent);
  R_EXPECT(node, 0) << "Getting row count of bad Node.";
  return node->children.size();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (!index.isValid() || role != Qt::EditRole) return false;
  Node *node = IndexToNode(index);
  R_EXPECT(node && node->parent && node->parent->BackingModel(), false);
  RepeatedMessageModel* siblings = node->parent->BackingModel()->TryCastAsRepeatedMessageModel();
  R_EXPECT(siblings, false);
  MessageModel* model = siblings->GetSubModel(node->row_in_parent)->TryCastAsMessageModel();
  QString oldName = model->Data(FieldPath::Of<buffers::TreeNode>(buffers::TreeNode::kNameFieldNumber)).toString();
  buffers::TreeNode::TypeCase type = (buffers::TreeNode::TypeCase)model->OneOfType("type");
  R_EXPECT(MainWindow::resourceMap->ValidName(type, value.toString()), false) << "Invalid resource name";
  bool ret = model->SetData(FieldPath::Of<buffers::TreeNode>(buffers::TreeNode::kNameFieldNumber), value);
  if (!ret) return false;
  node->DataChanged();
  emit ItemRenamed(type, oldName, value.toString());
  return true;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return {};
  R_EXPECT(IndexToNode(index), QVariant()) << "Failed to convert index:" << index << "to node";
  switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole: return IndexToNode(index)->display_name;
    case Qt::DecorationRole: return IndexToNode(index)->display_icon;
  }
  return {};
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);
  if (index.isValid()) flags |= Qt::ItemIsDragEnabled | Qt::ItemIsEditable;
  Node* node = IndexToNode(index);
  if (node && node->IsRepeated())
    flags |= Qt::ItemIsDropEnabled;
  return flags;
}

Qt::DropActions TreeModel::supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

QStringList TreeModel::mimeTypes() const { return mime_types_; }

QMimeData *TreeModel::mimeData(const QModelIndexList &indexes) const {
  QModelIndexList sortedIndexes = indexes;
  // Serialize the indexes in ascending order so we can make assumptions
  std::sort(sortedIndexes.begin(), sortedIndexes.end(),
            [](QModelIndex &a, QModelIndex &b) { return a.row() < b.row(); });

  QMimeData *mimeData = new QMimeData();
  QByteArray encodedData;
  QDataStream stream(&encodedData, QIODevice::WriteOnly);

  foreach (const QModelIndex &index, sortedIndexes) {
    if (!index.isValid()) continue;

    // Build the path bottom-up to put it into normal form
    QStack<int> path;
    path.push(index.row());
    QModelIndex parent = index.parent();
    while (parent.isValid()) {
      path.push(parent.row());
      parent = parent.parent();
    }

    // Serialize the path on the stack
    stream << path.pop();
    while (!path.isEmpty()) {
      stream << QString("/") << path.pop();
    }
    stream << QString(";");
  }

  mimeData->setData(mimeTypes()[0], encodedData);
  return mimeData;
}

bool TreeModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column,
                             const QModelIndex &parent) {
  if (action != Qt::MoveAction && action != Qt::CopyAction) return false;
  // ensure the data is in the format we expect
  if (!mimeData->hasFormat(mimeTypes()[0])) return false;
  QByteArray data = mimeData->data(mimeTypes()[0]);
  QDataStream stream(&data, QIODevice::ReadOnly);

  Node *parentNode = IndexToNode(parent);
  if (!parentNode) parentNode = root_.get();
  if (row == -1) row = rowCount(parent);
  QSet<const QModelIndex> nodes;
  std::vector<Message*> messages;

  while (!stream.atEnd()) {
    int itemRow = 0;
    stream >> itemRow;
    QModelIndex index = this->index(itemRow, 0, QModelIndex());
    QString delimiter = "";
    stream >> delimiter;
    while (delimiter != ";") {
      stream >> itemRow;
      index = this->index(itemRow, 0, index);
      stream >> delimiter;
    }
    nodes.insert(index);
  }
  nodes = GroupNodes(nodes);

  for (auto index : nodes) {
    auto node = IndexToNode(index);
    auto oldParent = index.parent();

    auto msg = node->GetMessage();
    Message* m_copy = msg.New();
    m_copy->CopyFrom(msg);
    messages.push_back(m_copy);

    if (action == Qt::MoveAction) {
      // offset the row to insert at by the number of
      // rows already removed from the same parent
      if (parent == oldParent && index.row() < row)
        --row;
    }
  }

  if (action == Qt::MoveAction)
    BatchRemove(nodes);

  for (auto* msg : messages) {
    parentNode->insert(*msg, row);
    delete msg;
  }

  return true;
}

QVariant TreeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  return tr("Name");
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
  if (column) return {};
  if (Node *n = IndexToNode(parent)) return n->index(row);
  return {};
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
  if (!index.isValid()) return QModelIndex();
  Node *const node = IndexToNode(index);
  R_EXPECT(node, QModelIndex()) << "Getting parent of bad index " << index << "...";
  Node *const parent = node->parent;
  if (!parent || !parent->parent) return {};
  return parent->parent->index(parent->row_in_parent);
}

QModelIndex TreeModel::Node::index(int row) const { return backing_tree->createIndex(row, 0, (void *)this); }
QString TreeModel::Node::DebugPath() const {
  if (backing_tree->IsValidNode(parent))
    return parent->DebugPath() + " → " + display_name;
  return display_name;
}

TreeModel::Node *TreeModel::IndexToNode(const QModelIndex &index) const {
  if (index.isValid() && index.internalPointer()) {
    Node *parent = static_cast<Node *>(index.internalPointer());
    R_EXPECT(IsValidNode(parent), nullptr) << "Dangling internal pointer to tree Node: " << parent;
    Node *node = parent->NthChild(index.row());
    if (!node) return nullptr;
    R_EXPECT(root_model_->ValidateSubModel(node->BackingModel()), nullptr)
        << "Tree contains a node (" << node->DebugPath() << ") with a dead model attached.";
    return node;
  } else {
    return root_.get();
  }
}

static void CollectNodes(const QModelIndex &node, QSet<const QModelIndex> &cache) {
  auto model = node.model();

  cache.insert(node);
  for (int r = 0; r < model->rowCount(node); ++r) {
    CollectNodes(model->index(r, 0, node), cache);
  }
}

void TreeModel::BatchRemove(const QSet<const QModelIndex> &indexes) {
  std::map<ProtoModel*, RepeatedMessageModel::RowRemovalOperation> removers;
  QVector<QPair<TreeNode::TypeCase, QString>> deletedResources;

  // Fully expand groups so we can correctly fire all necessary ItemRemoved signals
  QSet<const QModelIndex> selectedNodes;
  for (auto& index : qAsConst(indexes)) {
    CollectNodes(index, selectedNodes);
  }

  // Loop all selected
  for (auto& index : qAsConst(selectedNodes)) {
    Node* node = IndexToNode(index);
    R_ASSESS_C(node && node->BackingModel());
    MessageModel* m = node->BackingModel()->TryCastAsMessageModel();
    if (m) {
      emit ModelAboutToBeDeleted(m);
      MessageModel* parent = m->GetParentModel<MessageModel*>();
      R_ASSESS_C(parent);
      TreeNode::TypeCase type = (buffers::TreeNode::TypeCase)parent->OneOfType("type");
      deletedResources.append({type, index.data().toString()});
    }
  }

  // Don't delete children of deleted indexes
  QSet<const QModelIndex> nodes = GroupNodes(indexes);

  for (auto& index : qAsConst(nodes)) {
    Node* node = IndexToNode(index);
    R_ASSESS_C(node && node->BackingModel());

    ProtoModel *remove_me = node->BackingModel();
    RepeatedMessageModel *remove_from = nullptr;

    for (ProtoModel *ancestor = remove_me->GetParentModel(); ancestor; ancestor = remove_me->GetParentModel()) {
      if ((remove_from = ancestor->TryCastAsRepeatedMessageModel())) break;
      remove_me = ancestor;
    }

    R_ASSESS_C(remove_from);
    //TODO: Calling redundantly so delete/dnd last child of group doesn't spam warnings
    //RowRemovalOperation resets model which for some reason doesn't fix it, but this does
    //Calling begin/end remove fixes both cases
    beginRemoveRows(index.parent(), index.row(), index.row());
    removers.emplace(remove_from, remove_from).first->second.RemoveRow(remove_me->RowInParent());
    endRemoveRows();
  }

  for (auto& res : deletedResources)
    emit ItemRemoved(res.first, res.second, removers);

  // done with removers
  removers.clear();
}

// =====================================================================================================================
// == Tree Building ====================================================================================================
// =====================================================================================================================

static bool Describes(const FieldPath &field_path, int field_number) {
  return field_path.fields.size() == 1 && field_path.front()->number() == field_number;
}

TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent, MessageModel *model, int row_in_model)
    : backing_tree(backing_tree),
      parent(parent),
      backing_model(model),
      row_in_parent(row_in_parent),
      row_in_model(row_in_model) {
  RebuildFromModel(model, parent, row_in_parent);
}

void TreeModel::Node::RegisterDataListener() {
  updaters.push_back(connect(backing_model, &PrimitiveModel::dataChanged, [this]{
    if (!parent) backing_tree->beginResetModel();
    RebuildFromAnyModel(backing_model, parent, row_in_parent);
    UpdateParents();
    if (parent) {
      const QModelIndex ind = parent->index(row_in_parent);
      emit backing_tree->dataChanged(ind, ind);
    } else {
      backing_tree->endResetModel();
    }
  }));
}

void TreeModel::Node::RegisterRowListeners() {
  auto update = [this]{
    QModelIndex ind;
    if (parent) {
      ind = parent->index(row_in_parent);
      emit backing_tree->layoutAboutToBeChanged({ind});
    } else {
      backing_tree->beginResetModel();
    }
    RecursiveUndoPassThrough();
    this->RebuildFromAnyModel(backing_model, parent, row_in_parent);
    if (parent) {
      this->UpdateParents();
      emit backing_tree->dataChanged(ind, ind);
      emit backing_tree->layoutChanged({ind});
    } else {
      backing_tree->RebuildModelMapping();
      backing_tree->endResetModel();
    }
    AddSelfToMap(passthrough_node);
  };
  updaters.push_back(connect(backing_model, &ProtoModel::rowsInserted, update));
  updaters.push_back(connect(backing_model, &ProtoModel::modelReset, update));
  updaters.push_back(connect(backing_model, &ProtoModel::rowsRemoved, update));
  updaters.push_back(connect(backing_model, &ProtoModel::rowsMoved, update));
}


void TreeModel::Node::RebuildFromAnyModel(ProtoModel *model, Node *parent, int row_in_parent) {
  if (auto *m = model->TryCastAsMessageModel()) return RebuildFromModel(m, parent, row_in_parent);
  if (auto *m = model->TryCastAsRepeatedMessageModel()) return RebuildFromModel(m, parent, row_in_parent);
  if (auto *m = model->TryCastAsRepeatedModel()) return RebuildFromModel(m, parent, row_in_parent);
  if (auto *m = model->TryCastAsPrimitiveModel()) return RebuildFromModel(m, parent, row_in_parent);
  qDebug() << "Model type could not be inferred...";
}

void TreeModel::Node::RebuildFromModel(MessageModel *model, Node *parent, int row_in_parent) {
  // qDebug() << "Rebuild " << DebugPath();
  Reset(model, parent, row_in_parent);
  const auto &tree_meta = backing_tree->GetTreeDisplay(model->GetDescriptor()->full_name());
  const auto &msg_meta = BackingModel()->GetMessageDisplay(model->GetDescriptor()->full_name());
  if (tree_meta.custom_editor) {
    // Leaf node; connect the data change handler.
    RegisterDataListener();
    return;
  }
  for (int row = 0; row < model->rowCount(); ++row) {
    if (tree_meta.disable_oneof_reassignment && model->IsCulledOneofRow(row)) continue;
    int field_num = model->RowToField(row);
    if (Describes(msg_meta.label_field, field_num)) continue;
    // if (Describes(meta.icon_id_field, field_num)) continue;
    // if (Describes(meta.icon_file_field, field_num)) continue;
    PushChild(model->SubModelForRow(row), row);
  }
  is_passthrough = tree_meta.is_passthrough;
  PassThrough();
  // Nothing can modify rows of a MessageModel in a way that its TreeNode must handle.
  // Example: you cannot insert or remove rows of a message model. Edits will be handled by child models.
  // Register no data cchange handlers.
}

// Construct from Repeated Message Model.
TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent, RepeatedMessageModel *model,
                      int row_in_model)
    : backing_tree(backing_tree),
      parent(parent),
      backing_model(model),
      row_in_parent(row_in_parent),
      row_in_model(row_in_model) {
  RebuildFromModel(model, parent, row_in_parent);
}

void TreeModel::Node::RebuildFromModel(RepeatedMessageModel *model, Node *parent, int row_in_parent) {
  // qDebug() << "Rebuild " << DebugPath();
  Reset(model, parent, row_in_parent);
  for (int row = 0; row < backing_model->rowCount(); ++row) {
    PushChild(model->GetSubModel<ProtoModel>(row), row);
  }
  RegisterRowListeners();
}

// Construct from Repeated Primitive Model (base RepeatedModel).
TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent, RepeatedModel *model, int row_in_model)
    : backing_tree(backing_tree),
      parent(parent),
      backing_model(model),
      row_in_parent(row_in_parent),
      row_in_model(row_in_model) {
  RebuildFromModel(model, parent, row_in_parent);
}

void TreeModel::Node::RebuildFromModel(RepeatedModel *model, Node *parent, int row_in_parent) {
  Reset(model, parent, row_in_parent);
  for (int row = 0; row < model->rowCount(); ++row) {
    PushChild(model->GetSubModel(row), row);
  }
  RegisterRowListeners();
}

// Construct as a leaf node.
TreeModel::Node::Node(TreeModel *backing_tree, Node *parent, int row_in_parent, PrimitiveModel *model, int row_in_model)
    : backing_tree(backing_tree),
      parent(parent),
      backing_model(model),
      row_in_parent(row_in_parent),
      row_in_model(row_in_model) {
  RebuildFromModel(model, parent, row_in_parent);
}

void TreeModel::Node::RebuildFromModel(PrimitiveModel *model, Node *parent, int row_in_parent) {
  Reset(model, parent, row_in_parent);
}

void TreeModel::Node::Reset(ProtoModel *model, Node *parent, int row_in_parent) {
  UndoPassThrough();
  ClearListeners();
  children.clear();
  if (parent == this || (parent && parent->parent == this)) {
    qDebug() << "SEVERE: Cyclical parents: " << DebugPath() << " is being assigned " << parent->DebugPath();
  } else {
    this->parent = parent;
    this->row_in_parent = row_in_parent;
  }
  backing_model = model;
  is_passthrough = false;
  ComputeDisplayData();
}

void TreeModel::Node::DataChanged() {
  this->ComputeDisplayData();
}

void TreeModel::Node::PushChild(ProtoModel *model, int source_row) {
  R_EXPECT_V(model) << "Null model passed to TreeNode::PushChild()...";
  std::shared_ptr<Node> node;
  if (auto it = backing_tree->backing_nodes_.find(model); it != backing_tree->backing_nodes_.end()) {
    node = *it;
    if (node->backing_model != model) {
      qDebug() << "Model in map is stale; something bad is bound to happen.";
      node.reset();
    }
  }
  if (auto *sub_message = model->TryCastAsMessageModel()) {
    if (node)
      node->RebuildFromModel(sub_message, this, children.size());
    else
      node = std::make_unique<Node>(backing_tree, this, children.size(), sub_message, source_row);
  } else if (auto *repeated_message = model->TryCastAsRepeatedMessageModel()) {
    if (node)
      node->RebuildFromModel(repeated_message, this, children.size());
    else
      node = std::make_unique<Node>(backing_tree, this, children.size(), repeated_message, source_row);
  } else if (auto *repeated_message = model->TryCastAsRepeatedModel()) {
    if (node)
      node->RebuildFromModel(repeated_message, this, children.size());
    else
      node = std::make_unique<Node>(backing_tree, this, children.size(), repeated_message, source_row);
  } else if (auto *primitive_message = model->TryCastAsPrimitiveModel()) {
    if (node)
      node->RebuildFromModel(primitive_message, this, children.size());
    else
      node = std::make_unique<Node>(backing_tree, this, children.size(), primitive_message, source_row);
  } else {
    qDebug() << "Node has unknown type...";
    return;
  }
  children.push_back(node);
}

void TreeModel::Node::Print(int indent) const {
  for (int i = 0; i < indent; ++i) std::cout << ' ';
  std::cout << '"' << display_name.toStdString() << '"' << std::endl;
  for (auto &nn : children) nn->Print(indent + 2);
}

TreeModel::Node::~Node() {
  if (auto it = backing_tree->live_nodes.find(this); it != backing_tree->live_nodes.end())
    backing_tree->live_nodes.erase(it);
  ClearListeners();
}

void TreeModel::Node::ClearListeners() {
  for (const auto &connection : updaters) QObject::disconnect(connection);
  updaters.clear();
}
void TreeModel::Node::UpdateParents() {
  for (Node *p = parent; p; p = p->parent)
    p->ComputeDisplayData();
}

void TreeModel::Node::PassThrough() {
  if (!is_passthrough || children.size() != 1) return;
  passthrough_node = children[0];
  passthrough_node->ClearListeners();
  passthrough_node->parent = parent;
  passthrough_node->row_in_parent = row_in_parent;
  passthrough_model = backing_model;
  backing_model = passthrough_node->backing_model;
  children = passthrough_node->children;
  for (auto &child : children) {
    if (parent == child.get()) {
      qDebug() << "CRITICAL: Node parent loop. Disconnecting loop. Something bad will happen.";
    } else {
      child->parent = this;
    }
  }
  RegisterRowListeners();
  ComputeDisplayData();
}

void TreeModel::Node::UndoPassThrough() {
  if (!passthrough_node) return;
  ClearListeners();
  for (auto &child : children) {
    if (passthrough_node->parent == child.get()) {
      qDebug() << "CRITICAL: Node parent loop. Disconnecting loop. Something bad will happen.";
    } else {
      child->parent = passthrough_node.get();
    }
  }
  children.clear();
  children.push_back(passthrough_node);
  passthrough_node->parent = this;
  passthrough_node->row_in_parent = 0;
  passthrough_node->RegisterRowListeners();
  passthrough_node.reset();

  backing_model = passthrough_model;
  passthrough_model = nullptr;
  RegisterRowListeners();
}

void TreeModel::Node::RecursiveUndoPassThrough() {
  UndoPassThrough();
  for (auto &child : children) child->RecursiveUndoPassThrough();
}

// =====================================================================================================================
// == Node Mapping =====================================================================================================
// =====================================================================================================================

QModelIndex TreeModel::Node::mapFromSource(const QModelIndex &index) const {
  R_EXPECT(index.internalPointer(), QModelIndex()) << "Requested index " << index << " internal pointer is null";
  if (parent && index.internalPointer() == parent->backing_model) return parent->mapFromSource(index);
  R_EXPECT(index.internalPointer() == backing_model, QModelIndex()) << "Asked to map an unowned model index...";
  // For messages, translate field index() to child offset in children vector.
  if (backing_model->TryCastAsMessageModel()) {
    for (auto &child : children) {
      if (child->row_in_model == index.row()) return this->index(child->row_in_parent);
    }
    return {};
  }
  // All other index values should be 1:1.
  R_EXPECT(index.row() >= 0 && size_t(index.row()) < children.size(), QModelIndex())
      << "Requested index " << index << " is out of range.";
  return this->index(index.row());
}

QModelIndex TreeModel::mapFromSource(const QModelIndex &source_index) const {
  if (!source_index.isValid()) return {};
  if (!source_index.internalPointer()) {
    qDebug() << "Attempting to map a ProtoModel index with no internal pointer...";
    return {};
  }
  auto *const model = static_cast<ProtoModel *>(source_index.internalPointer());
  auto mapping = backing_nodes_.find(model);
  if (mapping == backing_nodes_.end()) return {};
  return (*mapping)->mapFromSource(source_index);
}

QModelIndex TreeModel::mapToSource(const QModelIndex &proxyIndex) const {
  if (!proxyIndex.isValid()) return {};
  if (Node *n = IndexToNode(proxyIndex)) {
    if (n->parent) return n->parent->index(n->row_in_parent);
  }
  return {};
}

// =====================================================================================================================
// == Display Configuration ============================================================================================
// =====================================================================================================================

static const std::string kEmptyString;
QString TreeModel::GetItemName(const Node *item) const { return item ? item->display_name : "<null>"; }
QVariant TreeModel::GetItemIcon(const Node *item) const { return item ? item->display_icon : QVariant(); }
TreeModel::Node *TreeModel::GetNthChild(Node *item, int n) const { return item ? item->NthChild(n) : nullptr; }
int TreeModel::GetChildCount(Node *item) const { return item ? item->children.size() : 0; }
const std::string &TreeModel::GetMessageType(const Node *node) { return node ? node->GetMessageType() : kEmptyString; }
TreeNode TreeModel::Node::GetMessage() const {
  auto *const model = passthrough_model ? passthrough_model : backing_model;
  auto *const message_model = model->TryCastAsMessageModel();
  auto *const buffer = static_cast<TreeNode*>(message_model->GetBuffer());
  return *buffer;
}

TreeModel::Node *TreeModel::Node::NthChild(int n) const {
  R_EXPECT(n >= 0 && (size_t)n < children.size(), nullptr)
      << "Accessing row " << n << " of a " << children.size() << "-row tree node `" << DebugPath() << "`";
  return children[n].get();
}
const std::string &TreeModel::Node::GetMessageType() const { return backing_model->GetDescriptor()->full_name(); }

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
  auto *display_model = passthrough_model ? passthrough_model : backing_model;
  display_name = display_model->GetDisplayName();
  if (auto *primitive_model = display_model->TryCastAsPrimitiveModel()) {
    if (const QVariant value = primitive_model->GetDirect(); !value.isNull()) {
      display_name += " = " + value.toString();
    }
  }
  display_icon = display_model->GetDisplayIcon();

  if (passthrough_node) {
    if (display_icon.isNull() || display_name.isEmpty()) passthrough_node->ComputeDisplayData();
    if (display_icon.isNull()) display_icon = passthrough_node->display_icon;
    if (display_name.isEmpty()) display_name = passthrough_node->display_name;
  }
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

QModelIndex TreeModel::duplicateNode(const QModelIndex & index) {
  Node *node = IndexToNode(index);
  if (node->IsRepeated()) {
    //return node->mapFromSource(node->repeated_model->duplicate(mapToSource(index)));
  }
  Node *parent = IndexToNode(index.parent());
  return node->duplicate(parent, node->row_in_parent+1);
}

void TreeModel::sortByName(const QModelIndex & index) {
  auto node = IndexToNode(index);
  if (!node) return;
  node->sort();
}

void TreeModel::triggerNodeEdit(const QModelIndex &index, QAbstractItemView *view) {
  R_EXPECT_V(index.isValid()) << "Invalid edit node selected";
  R_EXPECT_V(index.internalPointer()) << "Junk edit node selected";
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

void TreeModel::Node::sort() {
  if (!IsRepeated()) return;
  auto *const model = passthrough_model ? passthrough_model : backing_model;
  auto *const message_model = model->TryCastAsMessageModel();
  auto *const buffer = static_cast<TreeNode*>(message_model->GetBuffer());
  auto *child_field = buffer->mutable_folder()->mutable_children();
  std::sort(child_field->begin(), child_field->end(),
            [](const TreeNode &a, const TreeNode &b) { return a.name() < b.name(); });
  RebuildFromAnyModel(model, parent, row_in_parent);
  AddSelfToMap(passthrough_node);
}

QModelIndex TreeModel::Node::insert(const Message &message, int row) {
  auto *const repeated_message_model = backing_model->TryCastAsRepeatedMessageModel();
  R_EXPECT(repeated_message_model, QModelIndex()) << "Insert " << message.DebugString().c_str();
  return backing_tree->mapFromSource(repeated_message_model->insert(message, row));
}

QModelIndex TreeModel::Node::duplicate(Node* newParent, int row) {
  return newParent->insert(GetMessage(), row);
}

bool TreeModel::Node::IsRepeated() const { return backing_model->TryCastAsRepeatedModel(); }

ProtoModel *TreeModel::Node::BackingModel() const { return backing_model; }

const TreeModel::TreeNodeDisplayConfig &TreeModel::DisplayConfig::GetTreeDisplay(
    const std::string &message_qname) const {
  static const TreeModel::TreeNodeDisplayConfig sentinel;
  if (auto it = tree_display_configs_.find(message_qname); it != tree_display_configs_.end()) return *it;
  return sentinel;
}

const TreeModel::TreeNodeDisplayConfig &TreeModel::GetTreeDisplay(const std::string &message_qname) const {
  return display_config_.GetTreeDisplay(message_qname);
}
