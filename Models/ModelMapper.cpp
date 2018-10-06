#include "Models/ModelMapper.h"

#include "Editors/BaseEditor.h"

ModelMapper::ModelMapper(ProtoModel *model, BaseEditor *parent) : model(model) {
  mapper = new ImmediateDataWidgetMapper(parent);
  mapper->setOrientation(Qt::Vertical);
  mapper->setModel(model);
  parent->connect(model, &ProtoModel::dataChanged, parent, &BaseEditor::dataChanged);
}

// mapper

ProtoModel *ModelMapper::GetModel() { return model; }

void ModelMapper::addMapping(QWidget *widget, int section) { mapper->addMapping(widget, section); }

void ModelMapper::clearMapping() { mapper->clearMapping(); }

void ModelMapper::toFirst() { mapper->toFirst(); }

// model

void ModelMapper::RestoreBuffer() { model->RestoreBuffer(); }

void ModelMapper::ReplaceBuffer(google::protobuf::Message *buffer) { model->ReplaceBuffer(buffer); }

void ModelMapper::SetDirty(bool dirty) { model->SetDirty(dirty); }

bool ModelMapper::IsDirty() { return model->IsDirty(); }

int ModelMapper::rowCount(const QModelIndex &parent) const { return model->rowCount(parent); }

int ModelMapper::columnCount(const QModelIndex &parent) const { return model->columnCount(parent); }

bool ModelMapper::setData(const QModelIndex &index, const QVariant &value, int role) {
  return model->setData(index, value, role);
}

QVariant ModelMapper::data(int row, int column) const { return model->data(row, column); }

QVariant ModelMapper::data(const QModelIndex &index, int role) const { return model->data(index, role); }

ProtoModel *ModelMapper::GetSubModel(int fieldNum) { return model->GetSubModel(fieldNum); }

QString ModelMapper::GetString(int fieldNum, int index) { return model->GetString(fieldNum, index); }

QModelIndex ModelMapper::parent(const QModelIndex &index) const { return model->parent(index); }

QVariant ModelMapper::headerData(int section, Qt::Orientation orientation, int role) const {
  return model->headerData(section, orientation, role);
}

QModelIndex ModelMapper::index(int row, int column, const QModelIndex &parent) const {
  return model->index(row, column, parent);
}

Qt::ItemFlags ModelMapper::flags(const QModelIndex &index) const { return model->flags(index); }
