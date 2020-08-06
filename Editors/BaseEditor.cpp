#include "BaseEditor.h"

#include "Components/Logger.h"

#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>

BaseEditor::BaseEditor(EditorModel* model, QWidget* parent)
    : QWidget(parent), _model(model) {
  _model->setParent(this); // << take ownership
  _model->submit(); // << prepare initial backup
  _mapper = new EditorMapper(_model, this);
}

void BaseEditor::closeEvent(QCloseEvent* event) {
  if (isWindowModified()) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Resource Changed"),
                                  tr("Do you want to keep the changes?"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
      event->ignore();
      return;
    } else if (reply == QMessageBox::No) {
      _mapper->clearMapping(); // << mapper does not give a FUCK about changes
      // This should never happen but here incase someone decides to incorrectly null the backup
      R_ASSESS(_model->submit()) << "Failed to revert editor changes:"
                                 << this->windowTitle();
    }
  }

  event->accept();
}

void BaseEditor::OnSave() {
  // our editor model already mutated the super model so we
  // don't actually have to do anything and can just close
  this->parentWidget()->close();
}
