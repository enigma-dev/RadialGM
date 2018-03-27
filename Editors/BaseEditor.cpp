#include "BaseEditor.h"

#include <QCloseEvent>
#include <QMessageBox>

BaseEditor::BaseEditor(QWidget* parent, ProtoModel* model)
    : QWidget(parent), model(model), mapper(new ImmediateDataWidgetMapper(this)) {
  mapper->setOrientation(Qt::Vertical);
  mapper->setModel(model);
  connect(model, &ProtoModel::dataChanged, this, &BaseEditor::dataChanged);
}

void BaseEditor::closeEvent(QCloseEvent* event) {
  if (model->IsDirty()) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Unsaved Changes"), tr("Would you like to save the changes?"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
      event->ignore();
      return;
    } else if (reply == QMessageBox::No) {
      mapper->clearMapping();
      model->RestoreBuffer();
    }
  }

  event->accept();
}

void BaseEditor::ReplaceBuffer(google::protobuf::Message* buffer) { model->ReplaceBuffer(buffer); }

void BaseEditor::SetModelData(int index, const QVariant& value) {
  model->setData(model->index(index), value, Qt::DisplayRole);
}

QVariant BaseEditor::GetModelData(int index) { return model->data(model->index(index), Qt::DisplayRole); }

void BaseEditor::dataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/,
                             const QVector<int>& /*roles*/) {}
