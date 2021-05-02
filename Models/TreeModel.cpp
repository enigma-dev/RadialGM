#include "TreeModel.h"
#include "MainWindow.h"

#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "Models/ResourceModelMap.h"

#include <QCoreApplication>
#include <QItemSelectionModel>
#include <QMimeData>
#include <QStack>

TreeModel::TreeModel(MessageModel *root, QObject *parent, const DisplayConfig &config)
    : QAbstractItemModel(parent),
      mime_types_(GetMimeTypes(root->GetDescriptor())),
      display_config_(config),
      root_(Node::PassThrough(std::make_shared<Node>(this, nullptr, -1, root, -1))),
      root_model_(root) {
  RebuildModelMapping();
  connect(root, &MessageModel::modelReset, this, &TreeModel::DataBlownAway);
}

void TreeModel::SomeDataSomewhereChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &) {
  // TODO: probanly don't blow away the entire fucking tree when someone adds a subimage to a sprite
  emit TreeChanged(root_model_);
  beginResetModel();
  root_->RebuildFromModel(root_model_, nullptr, 0);
  RebuildModelMapping();
  endResetModel();
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
  backing_nodes_.insert(root_model_, root_);
  root_->AddChildrenToMap();
}

void TreeModel::Node::AddChildrenToMap() {
  backing_tree->live_nodes.insert(this);
   for (const auto &child : children) {
    backing_tree->backing_nodes_.insert(child->backing_model, child);
    child->AddChildrenToMap();
  }
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
  emit ItemRenamed(type, oldName, value.toString());
  return model->SetData(FieldPath::Of<buffers::TreeNode>(buffers::TreeNode::kNameFieldNumber), value);
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
  QHash<QModelIndex, unsigned> removedCount;

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

    qDebug() << index << delimiter;
    if (action != Qt::CopyAction) {
      auto oldParent = index.parent();

      // offset the row we are removing by the number of
      // rows already removed from the same parent
      if (parent != oldParent || row > itemRow) {
        itemRow -= removedCount[oldParent];
      }

      bool canDo = beginMoveRows(index.parent(), itemRow, itemRow, parent, row);
      if (!canDo) continue;

      // count this row as having been moved from this parent
      if (parent != oldParent || row > itemRow) removedCount[oldParent]++;

      // if moving the node within the same parent we need to adjust the row
      // since its own removal will affect the row we reinsert it at
      if (parent == oldParent && row > itemRow) --row;

      //auto oldRepeated = oldParent->mutable_child();
      //oldRepeated->ExtractSubrange(itemRow, 1, nullptr);
      //RepeatedFieldInsert<buffers::TreeNode>(parentNode->mutable_child(), node, row);
      //parents[node] = parentNode;
      endMoveRows();
      ++row;

      //emit ResourceMoved(node, oldParent);
    } else {
      //if (node->folder()) continue;
      //node = duplicateNode(*node);
      //insert(parent, row++, node);
    }
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

TreeModel::Node *TreeModel::IndexToNode(const QModelIndex &index) const {
  if (index.isValid() && index.internalPointer()) {
    Node *parent = static_cast<Node *>(index.internalPointer());
    R_EXPECT(live_nodes.find(parent) != live_nodes.end(), nullptr)
        << "Dangling internal pointer to tree Node: " << parent;
    Node *node = parent->NthChild(index.row());
    if (!node) return nullptr;
    R_EXPECT(root_model_->ValidateSubModel(node->BackingModel()), nullptr)
        << "Tree contains a node with a dead model attached.";
    return node;
  } else {
    return root_.get();
  }
}

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

void TreeModel::BatchRemove(const QSet<const QModelIndex> &indexes) {

  std::map<ProtoModel*, RepeatedMessageModel::RowRemovalOperation> removers;
  QVector<QPair<TreeNode::TypeCase, QString>> deletedResources;

  // Loop all selected
  for (auto& index : qAsConst(indexes)) {
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
    MessageModel* m = node->BackingModel()->TryCastAsMessageModel();
    RepeatedMessageModel* siblings;
    if (m) {
      MessageModel* tree_node = m->GetParentModel<MessageModel*>();
      R_ASSESS_C(tree_node);
      siblings = tree_node->GetParentModel<RepeatedMessageModel*>();
    } else { // is a folder I guess?
      R_ASSESS_C(node->parent && node->parent->BackingModel());
      siblings = node->parent->BackingModel()->TryCastAsRepeatedMessageModel();
    }
    R_ASSESS_C(siblings);
    removers.emplace(siblings, siblings).first->second.RemoveRow(node->row_in_parent);
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
    if (!parent) backing_tree->endResetModel();
    UndoPassThrough();
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
    if (!parent) backing_tree->beginResetModel();
    qDebug() << "Rebuild " << backing_model->DebugName() << " tree Node";
    std::cerr << "Rebuild " << backing_model->DebugName().toStdString() << " tree Node (named " << display_name.toStdString() << ")" << std::endl;
    UndoPassThrough();
    this->RebuildFromAnyModel(backing_model, parent, row_in_parent);
    this->UpdateParents();
    if (parent) {
      const QModelIndex ind = parent->index(row_in_parent);
      emit backing_tree->dataChanged(ind, ind);
      // emit backing_tree->layoutChanged({ind});
    } else {
      backing_tree->endResetModel();
    }
  };
  updaters.push_back(connect(backing_model, &ProtoModel::rowsInserted, update));
  updaters.push_back(connect(backing_model, &ProtoModel::modelReset, update));
  updaters.push_back(connect(backing_model, &ProtoModel::rowsRemoved, [this](QModelIndex, int first, int last) {
        QModelIndex ind;  // Get an index to this node. We are the parent.
        if (parent) ind = parent->index(row_in_parent);
        backing_tree->beginRemoveRows(ind, first, last);
        UndoPassThrough();
        qDebug() << "I have " << children.size() << " rows before rebuild";
        this->RebuildFromAnyModel(backing_model, parent, row_in_parent);
        qDebug() << "I have " << children.size() << " rows after rebuild";
        backing_tree->endRemoveRows();
        this->UpdateParents();
      }));
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
  children.clear();
  this->parent = parent;
  this->row_in_parent = row_in_parent;
  ClearListeners();
  backing_model = model;
  is_passthrough = false;
  ComputeDisplayData();
}

void TreeModel::Node::PushChild(ProtoModel *model, int source_row) {
  R_EXPECT_V(model) << "Null model passed to TreeNode::PushChild()...";
  std::shared_ptr<Node> node;
  if (auto it = backing_tree->backing_nodes_.find(model); it != backing_tree->backing_nodes_.end()) {
    node = *it;
    if (node->passthrough_node) {
      node = node->passthrough_node;
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
  children.push_back(PassThrough(node));
}

void TreeModel::Node::Print(int indent) const {
  for (int i = 0; i < indent; ++i) std::cout << ' ';
  std::cout << '"' << display_name.toStdString() << '"' << std::endl;
  for (auto &nn : children) nn->Print(indent + 2);
}

TreeModel::Node::~Node() {
  if (auto it = backing_tree->live_nodes.find(this); it != backing_tree->live_nodes.end())
    backing_tree->live_nodes.erase(it);
}

void TreeModel::Node::ClearListeners() {
  for (const auto &connection : updaters) QObject::disconnect(connection);
  updaters.clear();
}
void TreeModel::Node::UpdateParents() {
  for (Node *p = parent; p; p = p->parent) p->ComputeDisplayData();
}

std::shared_ptr<TreeModel::Node> TreeModel::Node::PassThrough(const std::shared_ptr<TreeModel::Node> node) {
  /*if (node->is_passthrough && node->children.size() == 1) {
    std::shared_ptr<TreeModel::Node> res = node->children[0];
    res->passthrough_node = node;
    res->parent = node->parent;
    res->row_in_parent = node->row_in_parent;
    node->children.clear();
    if (res->display_icon.isNull()) res->display_icon = node->display_icon;
    if (!node->display_name.isEmpty()) res->display_name = node->display_name;
    return res;
  }*/
  // node->PassThrough();
  return node;
}

void TreeModel::Node::PassThrough() {
  if (!is_passthrough || children.size() != 1) return;
  passthrough_node = children[0];
  children = passthrough_node->children;
  if (display_icon.isNull()) display_icon = passthrough_node->display_icon;
  if (!passthrough_node->display_name.isEmpty()) display_name = passthrough_node->display_name;
}

void TreeModel::Node::UndoPassThrough() {
  if (!passthrough_node) return;
  children.clear();
  children.push_back(passthrough_node);
  passthrough_node.reset();
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

TreeModel::Node *TreeModel::Node::NthChild(int n) const {
  R_EXPECT(n >= 0 && (size_t)n < children.size(), nullptr)
      << "Accessing row " << n << " of a " << children.size() << "-row tree node `" << display_name << "`";
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
  display_name = backing_model->GetDisplayName();
  if (auto *primitive_model = backing_model->TryCastAsPrimitiveModel()) {
    if (const QVariant value = primitive_model->GetDirect(); !value.isNull()) {
      display_name += " = " + value.toString();
    }
  }
  display_icon = backing_model->GetDisplayIcon();
}
/*
void TreeModel::Node::Absorb(TreeModel::Node &child) {
  // Our parent and row_in_parent remain unchanged.
  // But otherwise, we become exactly the givene child node.
  passthrough_node = backing_model;
  backing_model = child.backing_model;
  row_in_model = child.row_in_model;
  if (!child.display_icon.isNull()) display_icon = child.display_icon;
  children.swap(child.children);

  // Our parent doesn't change... but our children's parents sure do.
  for (auto &nchild : children) nchild->parent = this;
  // Hack: Doesn't maintain the same logic for leaf vs intermediate as the initial registration...
  // Determines whether the child was a leaf node by whether it's registered one event (dataChanged) or three.
  ClearListeners();
  if (child.updaters.size() == 1) {
    RegisterDataListener();
  } else {
    RegisterRowListeners();
  }
  child.ClearListeners();
}
*/
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

QModelIndex TreeModel::duplicateNode(const QModelIndex & /*index*/) {
  /*Node *node = IndexToNode(index);
  if (node->repeated_model) {
    return node->mapFromSource(node->repeated_model->duplicate(mapToSource(index)));
  }*/
  qDebug() << rowCount() << " rows";
  return QModelIndex();
}

void TreeModel::sortByName(const QModelIndex & /*index*/) { /*
  if (!index.isValid()) return;
  auto *node = static_cast<Message *>(index.internalPointer());
  if (!node) return;
  auto *child_field = node->mutable_child();
  std::sort(child_field->begin(), child_field->end(),
            [](const Message &a, const Message &b) { return a.name() < b.name(); });
  emit dataChanged(this->index(0, 0, index), this->index(node->child_size(), 0, index));*/
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

QModelIndex TreeModel::Node::insert(const Message &message, int row) {
  auto *const repeated_message_model = backing_model->TryCastAsRepeatedMessageModel();
  R_EXPECT(repeated_message_model, QModelIndex()) << "Insert " << message.DebugString().c_str();
  return backing_tree->mapFromSource(repeated_message_model->insert(message, row));
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
