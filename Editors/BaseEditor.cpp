#include "BaseEditor.h"
#include "Models/MessageModel.h"

#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>

BaseEditor::BaseEditor(MessageModel* resource_model, QWidget* parent)
    : QWidget(parent), _model(resource_model->GetParentModel<MessageModel>()), _nodeMapper(new ModelMapper(_model, this)) {
  _resMapper = new ModelMapper(resource_model, this);

  // Backup should be deleted by Qt's garbage collector when this editor is closed
  _resMapper->GetModel()->BackupModel(this);

  connect(_model, &QAbstractItemModel::modelReset, [this]() { this->RebindSubModels(); });
}

BaseEditor::~BaseEditor() {
  if (_reset_model_on_close) {
    _nodeMapper->clearMapping();
    if (!_resMapper->RestoreBackup()) {
      // This should never happen but here incase someone decides to incorrectly null the backup
      qDebug() << "Failed to revert editor changes";
    }
    _resMapper->clearMapping();
  }
}

void BaseEditor::closeEvent(QCloseEvent* event) {
  if (_resMapper->IsDirty()) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Unsaved Changes"), tr("Would you like to save the changes?"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
      event->ignore();
      return;
    } else if (reply == QMessageBox::No) {
      _reset_model_on_close = true;
    }
  }

  _resMapper->SetDirty(false);
  event->accept();
}

void BaseEditor::ReplaceBuffer(google::protobuf::Message* buffer) { _resMapper->ReplaceBuffer(buffer); }

void BaseEditor::dataChanged(const QModelIndex& topLeft, const QModelIndex& /*bottomRight*/, const QVariant& oldValue,
                             const QVector<int>& /*roles*/) {
  buffers::TreeNode* n = static_cast<buffers::TreeNode*>(_nodeMapper->GetModel()->GetBuffer());
  if (n == topLeft.internalPointer() && topLeft.row() == TreeNode::kNameFieldNumber) {
    this->setWindowTitle(QString::fromStdString(n->name()));
    emit ResourceRenamed(n->type_case(), oldValue.toString(), QString::fromStdString(n->name()));
  }
  _resMapper->SetDirty(true);
}

void BaseEditor::RebindSubModels() {
  _resMapper->toFirst();
  _nodeMapper->toFirst();
}

void BaseEditor::OnSave() {
  _resMapper->SetDirty(false);
  this->parentWidget()->close();
}
