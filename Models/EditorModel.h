#ifndef EDITORMODEL_H
#define EDITORMODEL_H

#include <QSortFilterProxyModel>
#include <QScopedPointer>

#include <google/protobuf/message.h>

using namespace google::protobuf;

/**
 * @brief The EditorModel class used to root and sandbox editor windows.
 * This model acts as a transformation of the super model rooted at an
 * editor's associated tree node. This prevents an editor from
 * accidentally corrupting other parts of the super model. This does not
 * preclude the editor from obtaining a super model index elsewhere and
 * editing other resources.
 */
class EditorModel : public QSortFilterProxyModel
{
  QScopedPointer<Message> _backup;

public:
  explicit EditorModel(const QModelIndex &protoRoot, QObject *parent);

  // these two overrides are how we move the root of this model
  // without causing the filter proxy model to map the rest
  // of the super model which it will never be editing anyway
  // then we only get data changed for indexes at or below our source root
  virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
  virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;

  const QPersistentModelIndex _protoRoot;

public slots:

  // these are for reverting the editor and since we edit the
  // super model directly, like GM, their meaning is somewhat
  // inverted to this editor model
  virtual void revert() override;
  virtual bool submit() override;
};

#endif // EDITORMODEL_H
