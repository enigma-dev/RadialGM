#include "BaseEdtior.h"

BaseEditor::BaseEditor(QWidget* parent, ResourceModel* model)
    : QWidget(parent), model(model), mapper(new ImmediateDataWidgetMapper(this)) {
  mapper->setOrientation(Qt::Vertical);
  connect(model, &ResourceModel::dataChanged, this, &BaseEditor::dataChanged);
  mapper->setModel(model);
}

QVariant BaseEditor::GetModelData(int index) { return model->data(model->index(index), Qt::DisplayRole); }

void BaseEditor::dataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/,
                             const QVector<int>& /*roles*/) {}
