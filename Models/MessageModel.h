#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include "ProtoModel.h"

#include <QHash>

// Model representing a protobuf message
class MessageModel : public ProtoModel {
  Q_OBJECT

  template<bool NO_DEFAULT>
  QVariant dataInternal(const QModelIndex &index, int role) const;

 public:
  MessageModel(ProtoModel *parent, Message *protobuf);
  MessageModel(QObject *parent, Message *protobuf);

  // On either intialization or restore of a model all
  // refrences to to the submodels it owns recursively must be updated
  void RebuildSubModels();

  // All editor changes are made instantly rather than on confirm.
  // Whenever an editor is spawned a copy of the underlying protobuf is made.
  // In the event the user opts to close the editor and undo their changes this backup is restored.
  MessageModel *GetBackupModel();
  MessageModel *BackupModel(QObject *parent);
  bool RestoreBackup();

  // Protobuf's can contain nested messages.
  // In message models these messages are held as submodels.
  // These messages can be accessed by the protobuf field number (ie Room::kInstancesFieldNumber)
  // FIXME: Sanity check this cast
  template <class T>
  T GetSubModel(int fieldNum) {
    return static_cast<T>(_subModels[fieldNum]);
  }

  // These are the same as the above but operate on the raw protobuf
  Message *GetBuffer();
  void ReplaceBuffer(Message *buffer);

  virtual bool SetData(const QVariant &value, int row, int column = 0) override;
  virtual QVariant Data(int row, int column = 0) const override;

  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
  virtual QVariant data(const QModelIndex &index, int role) const override;
  virtual QVariant dataOrDefault(const QModelIndex &index, int role) const;
  virtual QModelIndex parent(const QModelIndex &index) const override;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

 protected:
  MessageModel *_modelBackup;
  QScopedPointer<Message> _backupProtobuf;
  QHash<int, ProtoModel *> _subModels;
};

void UpdateReferences(MessageModel *model, const QString &type, const QString &oldName, const QString &newName);
QString ResTypeAsString(TypeCase type);

#endif
