#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "Components/ArtManager.h"
#include "Models/MessageModel.h"
#include "Models/RepeatedMessageModel.h"
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

class TreeModel : public QAbstractProxyModel {
  Q_OBJECT

 public:
  struct Node {
    /// When set, this node is a single message. Its children, if it has any, are its fields.
    MessageModel *const message_model = nullptr;
    /// When set, this node is a repeated field. Its children are messages within that field.
    RepeatedMessageModel *const repeated_model = nullptr;
    /// When set, this node is a leaf. It exists to index within a model. Use row_in_model for model operations.
    MessageModel *const containing_model = nullptr;
    QString displayName;
    QIcon displayIcon;
    Node *parent = nullptr;
    /// Generally a cache of this node's position in its parent node's list of children.
    /// This may not correspond 1:1 with the field mapping in the model. Use `row_in_model` for that.
    int row_in_parent = 0;
    /// The row number of this nodes's data in its model (e.g. containing_model).
    /// This may not correspond 1:1 with the node's position in its parent. Use `row_in_parent` for that.
    int row_in_model = 0;

    /// A cache of the children of this node, giving the models and metadata corresponding to each.
    QVector<QScopedPointer<Node>> children;

    bool SetName(const QString &name);
    Node *NthChild(int n) const;
    const std::string &GetMessageType();

    /// Builds the entire Node tree by copying the tree formed by the model.
    Node(MessageModel *model);
  };
  enum UserRoles {
    MessageTypeRole = Qt::UserRole
  };

  explicit TreeModel(MessageModel *root, QObject *parent);

  // ==================================================================================================================
  // == Display customization =========================================================================================
  // ==================================================================================================================

  /// Set the icon that will be used to display a given message node when no icon field is set on it.
  template<typename T> void SetDefaultIcon(const QString &icon_name) {
    SetDefaultIcon(T::descriptor()->full_name(), icon_name);
  }
  /// Hides a node when it only has one child, displaying the child instead.
  /// If both parent and child have a name, the namae is composited with /. Otherwise, only one name is displyed.
  template<typename T> void SetMessagePassthrough() {
    SetMessagePassthrough(T::descriptor()->full_name());
  }
  /// Associates a string field with the label of a given message.
  template<typename T> void SetMessageLabelField(int field_number) {
    SetMessageLabelField(T::descriptor()->full_name(), field_number);
  }
  /// Associates a string field with the icon of a given message. The field must contain a path to an image file.
  template<typename T> void SetMessageIconPathField(int field_number) {
    SetMessageIconPathField(T::descriptor()->full_name(), field_number);
  }
  /// Associates a string field with the icon name of a given message. The field must contain a partial icon name.
  /// The given pattern defines a prefix and suffix for that name, if required. This allows a value of "foo" to become
  /// "icon-foo-large", for example (to allow sandboxing icon sets for a given field).
  template<typename T> void SetMessageIconNameField(int field_number, const QString &pattern = "%s");
  /// Instead of editing each field in a given message, launch a special editor when trying to open it.
  template<typename Message, typename Editor, typename... ConstructionParams>
  void UseEditorWidget(ConstructionParams... construction_args) {
    UseEditorWidget(Message::descriptor()->full_name(), [&](MessageModel *model) {
      return new Editor(model, construction_args...);
    });
  }

  // These mirror the above, but are not compile-time safe.

  using EditorFactory = std::function<QWidget*(MessageModel*)>;
  void SetDefaultIcon(const std::string &message, const QString &icon_name);
  void SetMessagePassthrough(const std::string &message);
  void SetMessageIconPathField(const std::string &message, int field_number);
  void SetMessageLabelField(const std::string &message, int field_number);
  void SetMessageIconNameField(const std::string &message, int field_number, const QString &pattern = "%s");
  void UseEditorWidget(const std::string &message, EditorFactory factory);

  // ==================================================================================================================
  // == Data layer ====================================================================================================
  // ==================================================================================================================

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  // XXX: Do these need to be overridden, since the underlying model is to support them?
  Qt::DropActions supportedDropActions() const override;
  QStringList mimeTypes() const override;
  QMimeData *mimeData(const QModelIndexList &indexes) const override;
  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                    const QModelIndex &parent) override;

  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
  QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

  /// Inserts the given message as a child of the given parent index.
  QModelIndex insert(const QModelIndex &parent, int row, Node *node);
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
  /// and fires the ItemDeleted event on this proxy.
  void removeNode(const QModelIndex &index);
  /// Sorts the data in the specified node alphabetically. Fires the appropriate events on the backing model.
  void sortByName(const QModelIndex &index);

 signals:
  // Called when the name of a single TreeNode changes.
  void ItemRenamed(Node *node, const QString &oldName, const QString &newName);
  // Called when a resource (or group of resources) is moved.
  void ItemMoved(Node *node, TreeNode *old_parent);

 private:
  std::unique_ptr<Node> root_;

  struct FieldMeta {
    FieldPath label_field;
    FieldPath icon_field;
    QString icon_name;
    EditorFactory custom_editor;
    // When set, behaves like a whitelist. Only the fields listed here are used.
    QSet<const FieldDescriptor*> child_fields;
    // Fields listed here are ignored when enumerating children. Unused when child_fields are set.
    QSet<const FieldDescriptor*> non_child_fields;
  };

  QMap<std::string, FieldMeta> field_meta_;
  QString GetItemName(const Node *item) const;
  bool SetItemName(Node *item, const QString &name);
  QVariant GetItemIcon(const Node *item) const;
  Node *GetNthChild(Node *item, int n) const;
  int GetChildCount(Node *item) const;
  Node *IndexToNode(const QModelIndex &index) const;
  const std::string &GetMessageType(const Node *node);

  void SetupParents(Message *root);
  inline QString treeNodeMime() const { return QStringLiteral("RadialGM/TreeNode"); }
};

#endif  // TREEMODEL_H
