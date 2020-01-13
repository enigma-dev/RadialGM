#include "Models/ModelMapper.h"

#include "Editors/BaseEditor.h"

ModelMapper::ModelMapper(ProtoModelPtr model, BaseEditor *parent) : QObject(parent), model(model) {
  mapper = new ImmediateDataWidgetMapper(this);
  mapper->setOrientation(Qt::Vertical);
  mapper->setModel(model);
  parent->connect(model, &ProtoModel::dataChanged, parent, &BaseEditor::dataChanged);
}

// mapper

ProtoModelPtr ModelMapper::GetModel() { return model; }

void ModelMapper::addMapping(QWidget *widget, int section, QByteArray propName) {
  mapper->addMapping(widget, section, propName);
}

void ModelMapper::clearMapping() { mapper->clearMapping(); }

void ModelMapper::toFirst() { mapper->toFirst(); }

// model

void ModelMapper::ReplaceBuffer(google::protobuf::Message *buffer) { model->ReplaceBuffer(buffer); }

bool ModelMapper::RestoreBackup() { return model->RestoreBackup(); }

void ModelMapper::SetDirty(bool dirty) { model->SetDirty(dirty); }

bool ModelMapper::IsDirty() { return model->IsDirty(); }

int ModelMapper::rowCount(const QModelIndex &parent) const { return model->rowCount(parent); }

int ModelMapper::columnCount(const QModelIndex &parent) const { return model->columnCount(parent); }

bool ModelMapper::setData(const QModelIndex &index, const QVariant &value, int role) {
  return model->setData(index, value, role);
}

QVariant ModelMapper::data(int row, int column) const { return model->data(row, column); }

QVariant ModelMapper::data(const QModelIndex &index, int role) const { return model->data(index, role); }

RepeatedProtoModelPtr ModelMapper::GetRepeatedSubModel(int fieldNum) { return model->GetRepeatedSubModel(fieldNum); }

RepeatedStringModelPtr ModelMapper::GetRepeatedStringSubModel(int fieldNum) {
  return model->GetRepeatedStringSubModel(fieldNum);
}

ProtoModelPtr ModelMapper::GetSubModel(int fieldNum) { return model->GetSubModel(fieldNum); }

QModelIndex ModelMapper::parent(const QModelIndex &index) const { return model->parent(index); }

QVariant ModelMapper::headerData(int section, Qt::Orientation orientation, int role) const {
  return model->headerData(section, orientation, role);
}

QModelIndex ModelMapper::index(int row, int column, const QModelIndex &parent) const {
  return model->index(row, column, parent);
}

Qt::ItemFlags ModelMapper::flags(const QModelIndex &index) const { return model->flags(index); }
