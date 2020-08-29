#include "TimelineEditor.h"

#include "ui_CodeEditor.h"
#include "ui_TimelineEditor.h"

#include "CodeEditor.h"
#include "Dialogs/TimelineChangeMoment.h"

#include <QMessageBox>
#include <QPushButton>

TimelineEditor::TimelineEditor(MessageModel* model, QWidget* parent)
    : BaseEditor(model, parent), _ui(new Ui::TimelineEditor) {
   _ui->setupUi(this);

  _nodeMapper->addMapping(_ui->nameEdit, TreeNode::kNameFieldNumber);

  connect(_ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);

  connect(_ui->momentsList, &QAbstractItemView::clicked, [=](const QModelIndex& index) {
    SetCurrentEditor(index.row());
    _ui->stepBox->setValue(_momentsModel->Data(index.row(), Timeline::Moment::kStepFieldNumber).toInt());
  });

  connect(_ui->addMomentButton, &QPushButton::pressed, [=]() {
    AddMoment(_ui->stepBox->value());
    _ui->codeEditor->setDisabled(false);
    SetCurrentEditor(_momentsModel->rowCount() - 1);
    _ui->stepBox->setValue(_ui->stepBox->value() + 1);
  });

  connect(_ui->stepBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) { CheckDisableButtons(value); });

  connect(_ui->changeMomentButton, &QPushButton::pressed, [=]() {
    TimelineChangeMoment dialog(this);
    dialog.setWindowTitle((QString)("Change moment " + QString::number(_ui->stepBox->value()) + " to     "));
    dialog.SetSpinBoxValue(_ui->stepBox->value());
    if (dialog.exec()) ChangeMoment(IndexOf(_ui->stepBox->value()), dialog.GetSpinBoxValue());

    _ui->momentsList->reset();

    _ui->changeMomentButton->setDown(false);  // Qt buggy af
  });

  connect(_ui->deleteMomentButton, &QPushButton::pressed, [=]() {
    RemoveMoment(IndexOf(_ui->stepBox->value()));

    _ui->momentsList->reset();

    if (_momentsModel->rowCount() == 0) {
      CheckDisableButtons(-1);
      _ui->codeEditor->setDisabled(true);
    } else
      SetCurrentEditor(_momentsModel->rowCount() - 1);

    CheckDisableButtons(_ui->stepBox->value());
  });

  RebindSubModels();
}

TimelineEditor::~TimelineEditor() { delete _ui; }

void TimelineEditor::RebindSubModels() {
  _ui->codeEditor->ClearCodeWidgets();

  MessageModel* timelineModel = _model->GetSubModel<MessageModel*>(TreeNode::kTimelineFieldNumber);
  _momentsModel = timelineModel->GetSubModel<RepeatedMessageModel*>(Timeline::kMomentsFieldNumber);

  for (int moment = 0; moment < _momentsModel->rowCount(); ++moment) {
    BindMomentEditor(moment);
  }

  _ui->momentsList->setModel(_momentsModel);
  _ui->momentsList->setModelColumn(Timeline::Moment::kStepFieldNumber);

  if (_momentsModel->rowCount() == 0) _ui->codeEditor->setDisabled(true);

  CheckDisableButtons(_ui->stepBox->value());

  BaseEditor::RebindSubModels();
}

void TimelineEditor::AddMoment(int step) {
  int insertIndex = FindInsertIndex(step);
  _momentsModel->insertRow(insertIndex);
  _momentsModel->SetData(step, insertIndex, Timeline::Moment::kStepFieldNumber);
  BindMomentEditor(insertIndex);
}

void TimelineEditor::ChangeMoment(int oldIndex, int step) {
  if (IndexOf(step) != -1) {
    QMessageBox error;
    error.critical(this, "Error Changing Moment", "Moment already exists");
  } else {
    int newIndex = FindInsertIndex(step);
    _momentsModel->moveRows(oldIndex, 1, newIndex);
    _momentsModel->SetData(step, (newIndex < oldIndex) ? newIndex : newIndex - 1, Timeline::Moment::kStepFieldNumber);
  }
}

void TimelineEditor::RemoveMoment(int modelIndex) {
  RepeatedMessageModel::RowRemovalOperation remover(_momentsModel);
  remover.RemoveRow(modelIndex);
  _ui->codeEditor->RemoveCodeWidget(modelIndex);
}

int TimelineEditor::FindInsertIndex(int step) {
  int index = 0;
  while (index < _momentsModel->rowCount() &&
         step > _momentsModel->Data(index, Timeline::Moment::kStepFieldNumber).toInt()) {
    ++index;
  }

  return index;
}

int TimelineEditor::IndexOf(int step) {
  for (int r = 0; r < _momentsModel->rowCount(); ++r) {
    if (_momentsModel->Data(r, Timeline::Moment::kStepFieldNumber).toInt() == step) {
      return r;
    }
  }

  return -1;
}

void TimelineEditor::BindMomentEditor(int modelIndex) {
  CodeWidget* codeWidget = _ui->codeEditor->AddCodeWidget();
  ModelMapper* mapper(new ModelMapper(_momentsModel->GetSubModel<MessageModel*>(modelIndex), this));
  mapper->addMapping(codeWidget, Timeline::Moment::kCodeFieldNumber);
  mapper->toFirst();
}

void TimelineEditor::SetCurrentEditor(int modelIndex) {
  _ui->codeEditor->SetCurrentIndex(modelIndex);
  _ui->momentsList->selectionModel()->select(_momentsModel->index(modelIndex, Timeline::Moment::kStepFieldNumber),
                                             QItemSelectionModel::QItemSelectionModel::ClearAndSelect);
}

void TimelineEditor::CheckDisableButtons(int value) {
  for (int i = 0; i < _momentsModel->rowCount(); ++i) {
    if (_momentsModel->Data(i, Timeline::Moment::kStepFieldNumber).toInt() == value) {
      _ui->addMomentButton->setDisabled(true);
      _ui->changeMomentButton->setDisabled(false);
      _ui->deleteMomentButton->setDisabled(false);
      return;
    }
  }
  _ui->addMomentButton->setDisabled(false);
  _ui->changeMomentButton->setDisabled(true);
  _ui->deleteMomentButton->setDisabled(true);
}
