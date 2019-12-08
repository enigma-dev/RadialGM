#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include "treenode.pb.h"

#include "RepeatedProtoModel.h"

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

 public:
  explicit ProtoModel(google::protobuf::Message *protobuf, QObject* parent);
  explicit ProtoModel(google::protobuf::Message *protobuf, ProtoModelPtr parent);
  ~ProtoModel();
  ProtoModelPtr GetParentModel() const;
  ProtoModelPtr BackupModel(QObject* parent);
  ProtoModelPtr GetBackupModel();
  bool RestoreBackup();
  void ReplaceBuffer(google::protobuf::Message *buffer);
  google::protobuf::Message *GetBuffer();
  void SetDirty(bool dirty);
  bool IsDirty();
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QVariant data(int row, int column=0) const;
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
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                           const QVariant &oldValue = QVariant(0), const QVector<int> &roles = QVector<int>());

 private:
  ProtoModelPtr parentModel;
  ProtoModelPtr modelBackup;
  QHash<int, ProtoModelPtr> subModels;
  QHash<int, QList<QString>> repeatedStrings;
  QHash<int, RepeatedProtoModelPtr> repeatedModels;
  bool dirty;
  google::protobuf::Message *protobuf;
  QScopedPointer<Message> backupProtobuf;
};

void UpdateReferences(ProtoModelPtr model, const QString& type, const QString& oldName, const QString& newName);
QString ResTypeAsString(TypeCase type);

#endif  // RESOURCEMODEL_H
