#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include "ProtoModel.h"
#include "Utils/FieldPath.h"

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

  template<typename T, typename RType = typename std::remove_pointer<T>::type, EnabeIfCastable<RType> = true>
  RType* GetSubModel(int fieldNum) const {
    auto it = _subModels.find(fieldNum);
    return it == _subModels.end() ? nullptr : (*it)->As<RType>();
  }

  // Returns a mapping of row number to submodel.
  // FIXME: row numbers should be dense.
  const QHash<int, ProtoModel *> &SubModels() const { return _subModels; }

  // These are the same as the above but operate on the raw protobuf
  Message *GetBuffer();
  void ReplaceBuffer(Message *buffer);
  const Descriptor *GetDescriptor() const { return _protobuf->GetDescriptor(); }

  bool SetData(const QVariant &value, int row, int column = 0) override;
  QVariant Data(int row, int column = 0) const override;
  bool SetData(const FieldPath &field_path, const QVariant &value) override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QVariant dataOrDefault(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QModelIndex parent(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;

  // Casting.
  MessageModel *AsMessageModel() override { return this; }

 protected:
  MessageModel *_modelBackup;
  QScopedPointer<Message> _backupProtobuf;
  QHash<int, ProtoModel *> _subModels;
};

void UpdateReferences(MessageModel *model, const QString &type, const QString &oldName, const QString &newName);
QString ResTypeAsString(TypeCase type);

#endif
