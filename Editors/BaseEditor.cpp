#include "BaseEditor.h"
#include "Models/MessageModel.h"

#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>

BaseEditor::BaseEditor(MessageModel* treeNodeModel, QWidget* parent)
    : QWidget(parent), _nodeMapper(new ModelMapper(treeNodeModel, this)), _model(treeNodeModel) {
  buffers::TreeNode* n = static_cast<buffers::TreeNode*>(treeNodeModel->GetBuffer());
  _resMapper = new ModelMapper(treeNodeModel->GetSubModel<MessageModel*>(ResTypeFields[n->type_case()]), this);

  // Backup should be deleted by Qt's garbage collector when this editor is closed
  _resMapper->GetModel()->BackupModel(this);

  connect(_model, &QAbstractItemModel::modelReset, [this]() { this->RebindSubModels(); });

  connect(this, &BaseEditor::FocusGained, [=]() { _hasFocus = true; });
  connect(this, &BaseEditor::FocusLost, [=]() { _hasFocus = false; });
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
      _nodeMapper->clearMapping();
      if (!_resMapper->RestoreBackup()) {
        // This should never happen but here incase someone decides to incorrectly null the backup
        qDebug() << "Failed to revert editor changes";
      }
      _resMapper->clearMapping();
    }
  }

  _resMapper->SetDirty(false);
  event->accept();
}

bool BaseEditor::HasFocus() { return _hasFocus; }

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
