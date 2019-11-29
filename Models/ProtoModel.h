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
  explicit ProtoModel(google::protobuf::Message *protobuf, QObject *parent);
  ~ProtoModel();

  ProtoModel* Copy(QObject *parent);
  void ReplaceBuffer(google::protobuf::Message *buffer);
  google::protobuf::Message *GetBuffer();
  void SetDirty(bool dirty);
  bool IsDirty();
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QVariant data(int row, int column=0) const;
  QVariant data(const QModelIndex &index, int role) const override;
  RepeatedProtoModel* GetRepeatedSubModel(int fieldNum);
  ProtoModel *GetSubModel(int fieldNum);
  ProtoModel *GetSubModel(int fieldNum, int index);
  QString GetString(int fieldNum, int index) const;
  QModelIndex parent(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;

 signals:
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                           const QVariant &oldValue = QVariant(0), const QVector<int> &roles = QVector<int>());

 private:
  QHash<int, QVariant> messages;  //where QVariant is ProtoModel*
  QHash<int, QList<QVariant>> repeatedMessages;
  QHash<int, RepeatedProtoModel*> repeatedModels;
  bool dirty;
  google::protobuf::Message *protobuf;
};

void UpdateReferences(ProtoModel* model, const QString& type, const QString& oldName, const QString& newName);
QString ResTypeAsString(TypeCase type);

#endif  // RESOURCEMODEL_H
