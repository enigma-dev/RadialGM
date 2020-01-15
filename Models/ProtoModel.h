#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include "treenode.pb.h"

#include <QAbstractItemModel>
#include <QDebug>
#include <QHash>
#include <QList>

using namespace google::protobuf;

using TypeCase = buffers::TreeNode::TypeCase;
using TreeNode = buffers::TreeNode;
using Background = buffers::resources::Background;
using Font = buffers::resources::Font;
using Object = buffers::resources::Object;
using Path = buffers::resources::Path;
using Room = buffers::resources::Room;
using Sound = buffers::resources::Sound;
using Sprite = buffers::resources::Sprite;
using Timeline = buffers::resources::Timeline;

class ProtoModel;
using ProtoModelPtr = ProtoModel *;

// This is a parent to all internal models
class ProtoModel : public QAbstractItemModel {
 public:
  explicit ProtoModel(ProtoModelPtr parent);

  // The parent model is the model that own's the current model
  // For resource models like a Room this will be a nullptr
  // For instances it would be a pointer to the room
  // For *a* instance it would be a pointer to a room's instances feild's model
  ProtoModelPtr GetParentModel() const;

  // If a submodel changed technically any model that owns it has also changed
  // so we need to notify all parents when anything changes in their descendants
  void ParentDataChanged();

  // All editor changes are made instantly rather than on confirm.
  // Whenever an editor is spawned a copy of the underlying protobuf is made.
  // In the event the user opts to close the editor and undo their changes this backup is restored.
  ProtoModelPtr GetBackupModel();
  ProtoModelPtr BackupModel(QObject *parent);
  bool RestoreBackup();

  // These are the same as the above but operate on the raw protobuf
  Message *GetBuffer();
  void ReplaceBuffer(Message *buffer);

  // Protobuf's can contain nested messages.
  // In message models these messages are held as submodels.
  // These messages can be accessed by the protobuf field number (ie Room::kInstancesFieldNumber)
  // In the case of repeated models you would access a submodel by it's position within the data structure.
  // (ie instancesModel->GetSubmodel(3))
  template <class T>
  T GetSubModel(int fieldNum) {
    return static_cast<T>(_subModels[fieldNum]);
  }

  // A model is "dirty" if the user has made any changes to it since opening the editor.
  void SetDirty(bool dirty);
  bool IsDirty();

  // Convience functions to access data without constructing a QModelIndex.
  // (equiv. to  data(model->index(row, col), Qt::DisplayRole)
  // Row is field number (ie Room::kInstancesFieldNumber)
  // Column is to access a specific model in the case of a repeated feild
  QVariant Data(int row, int column = 0) const;
  bool SetData(const QVariant &value, int row, int column = 0);

  // From here down marks QAbstractItemModel functions required to be implemented
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override = 0;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override = 0;
  virtual QVariant data(const QModelIndex &index, int role) const override = 0;
  virtual QModelIndex parent(const QModelIndex &index) const override = 0;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override = 0;
  virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override = 0;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

 signals:
  // QAbstractItemModel has a datachanged signal but it doesn't store the old values
  // We use old values in some places to revert invalid changes
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVariant &oldValue = QVariant(0),
                   const QVector<int> &roles = QVector<int>());

 protected:
  bool _dirty;
  QVector<ProtoModelPtr> _subModels;
  Message *_protobuf;
  ProtoModelPtr _parentModel;
  ProtoModelPtr _modelBackup;
  QScopedPointer<Message> _backupProtobuf;
};

#endif
