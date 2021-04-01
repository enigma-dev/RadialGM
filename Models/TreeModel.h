#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "Components/ArtManager.h"
#include "Models/MessageModel.h"
#include "Models/RepeatedMessageModel.h"
#include "Models/PrimitiveModel.h"
#include "Utils/FieldPath.h"
#include "treenode.pb.h"

#include <QAbstractItemView>
#include <QAbstractProxyModel>
#include <QHash>
#include <QVector>

#include <memory>
#include <unordered_map>

using TypeCase = buffers::TreeNode::TypeCase;
using IconMap = std::unordered_map<TypeCase, QIcon>;

class TreeModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  using EditorLauncher = std::function<void(MessageModel*)>;

 private:
  struct TreeNodeDisplayConfig {
    // When true, removes this node from the tree if it only has one child.
    // The child node is displayed directly in its place.
    bool is_passthrough = false;
    // When true, culls oneof children that are not set when one of them actually is,
    // preventing user reassignment of the oneof type case (and destruction of oneof data).
    bool disable_oneof_reassignment = false;
    // Factory method to create an editor for this entity. When specified, no children will be populated.
    EditorLauncher custom_editor;
    // When set, behaves like a whitelist. Only the fields listed here are used.
    QSet<int> child_rows;
    // Fields listed here are ignored when enumerating children. Unused when child_rows are set.
    QSet<int> non_child_rows;
  };

 public:
  struct Node {
    TreeModel *const backing_tree;
    Node *parent = nullptr;

   private:
    ProtoModel *backing_model;
    /// The signal connection that updates this node when data changes, if applicable.
    /// Stored only for leaf nodes.
    std::vector<QMetaObject::Connection> updaters;

   public:
    /// Cache of the name (or value) field of the underlying proto.
    QString display_name;
    /// Cache of the icon field or per-message display icon of the underlying proto.
    QIcon display_icon;
    /// Generally a cache of this node's position in its parent Node's list of children (parent->children).
    /// This may not correspond 1:1 with the field mapping in the model. Use `row_in_model` for that.
    int row_in_parent = 0;
    /// The row number of this node's data in its model (i.e. parent->backing_model).
    /// This may not correspond 1:1 with the node's position in its parent. Use `row_in_parent` for that.
    int row_in_model = 0;

    /// A cache of the children of this node, giving the models and metadata corresponding to each.
    std::vector<std::shared_ptr<Node>> children;

    bool SetName(const QString &name, const ProtoModel::MessageDisplayConfig &meta);
    Node *NthChild(int n) const;
    const std::string &GetMessageType() const;
    QModelIndex mapFromSource(const QModelIndex &index) const;
    QModelIndex index(int row) const;
    QModelIndex insert(const Message &message, int row);

    /// Returns whether this node represents a repeated field.
    bool IsRepeated() const;
    ProtoModel *BackingModel() const;

    /// Debug print.
    void Print(int indent = 0) const;

    /// Builds the entire Node tree by copying the tree formed by the model.
    Node(TreeModel *backing_tree, Node *parent, int row_in_parent, MessageModel *model, int row_in_model);
    /// Builds more Node tree from each item in a repeated model.
    Node(TreeModel *backing_tree, Node *parent, int row_in_parent, RepeatedModel *model, int row_in_model);
    /// Builds more Node tree from each message in a repeated model.
    Node(TreeModel *backing_tree, Node *parent, int row_in_parent, RepeatedMessageModel *model, int row_in_model);
    /// Constructs as a leaf node. The specified field should not be a message.
    Node(TreeModel *backing_tree, Node *parent, int row_in_parent, PrimitiveModel *model_row, int row_in_model);
    /// Removes self from the live set.
    ~Node();

    void RebuildFromModel(MessageModel *model);
    void RebuildFromModel(RepeatedModel *model);
    void RebuildFromModel(RepeatedMessageModel *model);
    void RebuildFromModel(PrimitiveModel *model);

    /// Adds this node to the containing tree's model map.
    void AddChildrenToMap();

   private:
    void PushChild(ProtoModel *model, int source_row);
    void ComputeDisplayData();
    void Absorb(Node &child);

    /// Rebuilds parent nodes' display data.
    void UpdateParents();
    /// Removes active listeners so that they can be repopulated for a new model.
    void ClearListeners();
  };

  enum UserRoles {
    MessageTypeRole = Qt::UserRole
  };

  // ===================================================================================================================
  // == Display customization ==========================================================================================
  // ===================================================================================================================

  struct DisplayConfig {
    /// Hides a node when it only has one child, displaying the child instead.
    /// If both parent and child have a name, the namae is composited with /. Otherwise, only one name is displyed.
    template<typename T> void SetMessagePassthrough() {
      SetMessagePassthrough(T::descriptor()->full_name());
    }
    /// Disables reassignment of oneof values for nodes which already have a oneof choice selected.
    /// Effectively, this hides oneof values except for the one that is present.
    template<typename T> void DisableOneofReassignment() {
      DisableOneofReassignment(T::descriptor()->full_name());
    }

    /// Instead of editing each field in a given message, launch a special editor when trying to open it.
    /// The constructor of your editor must accept a MessageModel by pointer. It can accept additional arguments,
    /// as well (e.g. MyEditor(model, my_argument, my_other_argument). In this case, invoke this method like so:
    ///
    /// display_config.UseEditorWidget<MessageToEdit, MyEditor>(my_argument, my_other_argument);
    template<typename Message, typename Editor, typename... ConstructionParams>
    void UseEditorWidget(ConstructionParams... construction_args) {
      UseEditorWidget(Message::descriptor()->full_name(), [=](MessageModel *model) {
        new Editor(model, construction_args...);
      });
    }

    /// Similar to UseEditorWidget, but the given function is invoked with the model to be edited.
    template<typename Message> void UseEditorLauncher(EditorLauncher editor_launcher) {
      UseEditorWidget(Message::descriptor()->full_name(), editor_launcher);
    }

    // Fetch metadata (or the default instance) by its qualified message name.
    const TreeNodeDisplayConfig &GetTreeDisplay(const std::string &message_qname) const;

   private:
    // These mirror the above, but are not compile-time safe.
    void SetMessagePassthrough(const std::string &message);
    void DisableOneofReassignment(const std::string &message);
    void UseEditorWidget(const std::string &message, EditorLauncher launcher);

    QMap<std::string, TreeNodeDisplayConfig> tree_display_configs_;
  };

  explicit TreeModel(MessageModel *root, QObject *parent, const DisplayConfig &config);

  // ===================================================================================================================
  // == Data layer =====================================================================================================
  // ===================================================================================================================

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
  QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

  /// Inserts the given message as a child of the given parent index.
  QModelIndex insert(const QModelIndex &parent, int row, const Message &message);
  /// Inserts the given message as a child of the given parent index.
  QModelIndex addNode(const Message &child, const QModelIndex &parent);
  /// Duplicates the given node, placing it immediately after the original in sequence.
  /// Does NOT launch an editor. To launch the appropriate editor (or simply trigger a rename of the duplicated node),
  /// call triggerNodeEdit() on the result.
  QModelIndex duplicateNode(const QModelIndex &index);
  /// Begins editing the specified node in the UI. For label-only nodes, such as folders, this is a rename.
  /// For valued nodes, launches the specified editor or begins editing the value column.
  void triggerNodeEdit(const QModelIndex &index, QAbstractItemView *view);
  /// Erases the node at the given index from the model. Triggers the appropriate events on the backing model,
  /// Sorts the data in the specified node alphabetically. Fires the appropriate events on the backing model.
  void sortByName(const QModelIndex &index);

  // Mimedata stuff required for Drag & Drop and clipboard functions
  Qt::DropActions supportedDropActions() const override;
  QStringList mimeTypes() const override;
  QMimeData *mimeData(const QModelIndexList &indexes) const override;
  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                    const QModelIndex &parent) override;

  Node *IndexToNode(const QModelIndex &index) const;
  void BatchRemove(const QSet<const QModelIndex> &indexes);

  // Slots
 public slots:
  void SomeDataSomewhereChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
  void DataBlownAway();

 signals:
  void TreeChanged(MessageModel* root);
  // Called when the name of a single TreeNode changes.
  void ItemRenamed(TreeNode::TypeCase type, const QString &oldName, const QString &newName);
  // Called when a resource (or group of resources) is moved.
  void ItemMoved(TreeModel::Node *node, TreeNode *old_parent);
  // Called when a resource (or group of resources) is removed.
  void ItemRemoved(TreeNode::TypeCase type, const QString &name, std::map<ProtoModel*,
                   RepeatedMessageModel::RowRemovalOperation>& removers);
  void ModelAboutToBeDeleted(MessageModel *m);

 private:
  /// Set of all living nodes belonging to this tree.
  std::set<Node*> live_nodes;
  /// Map from backing model to the node representing it.
  QHash<ProtoModel*, std::shared_ptr<Node>> backing_nodes_;

  /// Cache of supported mime types.
  QStringList mime_types_;
  /// Display metadata for messages in this tree.
  DisplayConfig display_config_;
  // Warning: this must be initialized *after* the above two maps.
  std::shared_ptr<Node> root_;
  MessageModel *root_model_;

  QString GetItemName(const Node *item) const;
  bool SetItemName(Node *item, const QString &name);
  QVariant GetItemIcon(const Node *item) const;
  Node *GetNthChild(Node *item, int n) const;
  int GetChildCount(Node *item) const;
  const std::string &GetMessageType(const Node *node);

  void RebuildModelMapping();

  // Retrieve field metadata for a tree. Returns a sentinel if not specified.
  const TreeNodeDisplayConfig &GetTreeDisplay(const std::string &message_qname) const;
};

#endif  // TREEMODEL_H
