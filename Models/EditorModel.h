#ifndef EDITORMODEL_H
#define EDITORMODEL_H

#include <QIdentityProxyModel>

/**
 * @brief The EditorModel class used to root and sandbox editor windows.
 * This model acts as a transformation of the super model rooted at an
 * editor's associated tree node. This prevents an editor from
 * accidentally corrupting other parts of the super model. This does not
 * preclude the editor from obtaining a super model index elsewhere and
 * editing other resources.
 */
class EditorModel : public QIdentityProxyModel
{
  const QPersistentModelIndex &protoRoot;

public:
  explicit EditorModel(const QPersistentModelIndex &protoRoot, QObject *parent);

  // these two overrides are how we move the root of this model
  // without causing the identity proxy model to map the rest
  // of the super model which it will never be editing anyway
  virtual QModelIndex mapToSource(const QModelIndex &sourceIndex) const override;
  virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
};

#endif // EDITORMODEL_H
