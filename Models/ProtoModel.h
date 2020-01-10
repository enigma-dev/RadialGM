#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include "treenode.pb.h"

#include "RepeatedProtoModel.h"

#include <QDebug>
#include <QHash>
#include <QList>

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

class ProtoModel : public QAbstractItemModel {
  Q_OBJECT

 protected:
  void ParentDataChanged();
  struct SubModels {
    void Clear();
    QHash<int, ProtoModelPtr> protoModels;
    QHash<int, QList<QString>> strings;
    QHash<int, RepeatedProtoModelPtr> repeatedModels;
  };

  bool dirty;
  Message *protobuf;
  SubModels subModels;
  ProtoModelPtr parentModel;
  ProtoModelPtr modelBackup;
  QScopedPointer<Message> backupProtobuf;

 public:
  explicit ProtoModel(Message *protobuf, QObject *parent);
  explicit ProtoModel(Message *protobuf, ProtoModelPtr parent);
  void RebuildSubModels();
  ProtoModelPtr GetParentModel() const;
  ProtoModelPtr BackupModel(QObject *parent);
  ProtoModelPtr GetBackupModel();
  SubModels &GetSubModels();
  bool RestoreBackup();
  void ReplaceBuffer(Message *buffer);
  Message *GetBuffer();
  void SetDirty(bool dirty);
  bool IsDirty();
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
  bool setData(int row, int column, const QVariant &value);
  QVariant data(int row, int column = 0) const;
  QVariant data(const QModelIndex &index, int role) const override;
  RepeatedProtoModelPtr GetRepeatedSubModel(int fieldNum);
  ProtoModelPtr GetSubModel(int fieldNum);
  ProtoModelPtr GetSubModel(int fieldNum, int index);
  QString GetString(int fieldNum, int index) const;
  QModelIndex parent(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;

 signals:
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVariant &oldValue = QVariant(0),
                   const QVector<int> &roles = QVector<int>());
};

void UpdateReferences(ProtoModelPtr model, const QString &type, const QString &oldName, const QString &newName);
QString ResTypeAsString(TypeCase type);

#endif  // RESOURCEMODEL_H
