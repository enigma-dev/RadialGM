#include "BaseEditor.h"

#include <QCloseEvent>
#include <QMessageBox>

BaseEditor::BaseEditor(ProtoModel* treeNodeModel, QWidget* parent) :
    QWidget(parent),
    nodeMapper(new ModelMapper(treeNodeModel, this)) {
    buffers::TreeNode* n = static_cast<buffers::TreeNode*>(treeNodeModel->GetBuffer());
    resMapper = new ModelMapper(treeNodeModel->GetSubModel(ResTypeFields[n->type_case()]), this);

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
      nodeMapper->clearMapping();
      nodeMapper->RestoreBuffer();
      resMapper->clearMapping();
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

void BaseEditor::dataChanged(const QModelIndex& topLeft, const QModelIndex& /*bottomRight*/,
                             const QVector<int>& /*roles*/) {
  buffers::TreeNode* n = static_cast<buffers::TreeNode*>(nodeMapper->GetModel()->GetBuffer());
  if (n == topLeft.internalPointer() && topLeft.row() == TreeNode::kNameFieldNumber) {
    //name changed
    std::cout << n->name() << std::endl;
  }
}
