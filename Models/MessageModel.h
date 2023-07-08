#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include "ProtoModel.h"
#include "Utils/FieldPath.h"
#include "Components/Logger.h"

#include <QHash>

// Model representing a protobuf message
class MessageModel : public ProtoModel {
  Q_OBJECT

  template<bool NO_DEFAULT>
  QVariant dataInternal(const QModelIndex &index, int role) const;

 public:
  MessageModel(ProtoModel *parent, Message *protobuf, int row_in_parent);
  MessageModel(ProtoModel *parent, const Descriptor *descriptor, int row_in_parent);
  MessageModel(NonProtoParent parent, Message *protobuf);

  // On either intialization or restore of a model all
  // refrences to to the submodels it owns recursively must be updated
  void RebuildSubModels();

  // All editor changes are made instantly rather than on confirm.
  // Whenever an editor is spawned a copy of the underlying protobuf is made.
  // In the event the user opts to close the editor and undo their changes this backup is restored.
  MessageModel *GetBackupModel();
  MessageModel *BackupModel(QObject *parent);
  bool RestoreBackup();

  template<typename T, EnableIfCastable<T> = true>
  auto* GetSubModel(int fieldNum) const {
    auto it = submodels_by_field_.find(fieldNum);
    return it == submodels_by_field_.end() ? nullptr : (*it)->As<T>();
  }

  QString GetDisplayName() const override;
  QIcon GetDisplayIcon() const override;

  const FieldDescriptor *GetRowDescriptor(int row) const override;

  // Returns true iff the specified row is an unset oneof field whose containing oneof has a different value specified.
  // In other words, tests whether the given row should be hidden because it isn't the selected option of some oneof.
  bool IsCulledOneofRow(int row) const;

  // Returns the index in the oneof
  int OneOfType(const std::string& name) const {
    const OneofDescriptor* o = GetDescriptor()->FindOneofByName(name);
    R_EXPECT(o && _protobuf, -1) << "Failed to get oneof index";
    return _protobuf->GetReflection()->GetOneofFieldDescriptor(*_protobuf, o)->number();
  }

  // Translates a row number from this model into the underlying Protocol Buffer tag (field number).
  int RowToField(int row) const { return descriptor_->field(row)->number(); }

  // Translates an underlying Protocol Buffer tag (field number) to the row number from this model.
  int FieldToRow(int field_number) const {
    const FieldDescriptor *field = descriptor_->FindFieldByNumber(field_number);
    if (field) return field->index();
    qDebug() << "Looking up bad field number " << field_number
             << " in MessageModel " << GetDescriptor()->full_name().c_str();
    return -1;
  }

  ProtoModel *SubModelForRow(int row) const {
    if (!_protobuf) return nullptr;
    if (row < 0 || row >= submodels_by_row_.size()) {
      qDebug() << "Accessing bad row " << row << " of " << descriptor_->name().c_str()
               << " (" << submodels_by_row_.size() << " rows)";
      return nullptr;
    }
    return submodels_by_row_[row];
  }

  // These are the same as the above but operate on the raw protobuf
  Message *GetBuffer();
  void ReplaceBuffer(const Message *buffer);
  // Does the fastest possible conversion from field to QString. Returns empty for message fields.
  QString FastGetQString(const FieldDescriptor *field) const;

  using ProtoModel::Data;
  using ProtoModel::SetData;
  QVariant Data() const override;
  bool SetData(const QVariant &value) override;
  const ProtoModel *GetSubModel(const FieldPath &field_path) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;

  // Casting.
  QString DebugName() const override { return QString::fromStdString("MessageModel<" + descriptor_->name() + ">"); }
  MessageModel *TryCastAsMessageModel() override { return this; }

 protected:
  google::protobuf::Message *_protobuf;
  MessageModel *_modelBackup = nullptr;
  QScopedPointer<Message> _backupProtobuf;
  QVector<ProtoModel *> submodels_by_row_;
  QHash<int, ProtoModel *> submodels_by_field_;
};

#endif
