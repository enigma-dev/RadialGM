#include "BaseEditor.h"

#include <QCloseEvent>
#include <QMessageBox>

BaseEditor::BaseEditor(ProtoModel* model, QWidget* parent)
    : QWidget(parent), resMapper(new ModelMapper(model, this)) {
}

void BaseEditor::closeEvent(QCloseEvent* event) {
  if (resMapper->IsDirty()) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Unsaved Changes"), tr("Would you like to save the changes?"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
      event->ignore();
      return;
    } else if (reply == QMessageBox::No) {
      resMapper->clearMapping();
      resMapper->RestoreBuffer();
    }
  }

  resMapper->SetDirty(false);
  event->accept();
}

void BaseEditor::ReplaceBuffer(google::protobuf::Message* buffer) { resMapper->ReplaceBuffer(buffer); }

void BaseEditor::SetModelData(int index, const QVariant& value) {
  resMapper->setData(resMapper->index(index), value, Qt::DisplayRole);
}

QVariant BaseEditor::GetModelData(int index) { return resMapper->data(resMapper->index(index), Qt::DisplayRole); }

void BaseEditor::dataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/,
                             const QVector<int>& /*roles*/) {}
