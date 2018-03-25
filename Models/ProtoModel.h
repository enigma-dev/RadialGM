#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include <QAbstractItemModel>

#include <google/protobuf/message.h>

class ProtoModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  ProtoModel(google::protobuf::Message *protobuf, QObject *parent = nullptr);
  void ReplaceBuffer(google::protobuf::Message *buffer);
  void RestoreBuffer();
  void SetDirty(bool dirty);
  bool IsDirty();
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QVariant data(int index) const;
  QVariant data(const QModelIndex &index, int role) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;

 private:
  bool dirty;
  google::protobuf::Message *protobuf;
  google::protobuf::Message *protobufBackup;
};

#endif  // RESOURCEMODEL_H
