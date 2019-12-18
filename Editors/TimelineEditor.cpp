#include "TimelineEditor.h"

#include "ui_TimelineEditor.h"
#include "ui_CodeEditor.h"

#include "Dialogs/TimelineChangeMoment.h"
#include "CodeEditor.h"

#include <QSplitter>
#include <QPushButton>
#include <QSizePolicy>
#include <QDebug>

TimelineEditor::TimelineEditor(ProtoModelPtr model, QWidget* parent)
    : BaseEditor(model, parent), codeEditor(new CodeEditor(this, true)),  ui(new Ui::TimelineEditor) {

  ProtoModelPtr timelineModel = model->GetSubModel(TreeNode::kTimelineFieldNumber);
  momentsModel = timelineModel->GetRepeatedSubModel(Timeline::kMomentsFieldNumber);

  QLayout* layout = new QVBoxLayout(this);
  QSplitter* splitter = new QSplitter(this);

  QWidget* momentWidget = new QWidget(this);
  ui->setupUi(momentWidget);

  connect(ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);

  splitter->addWidget(momentWidget);
  splitter->addWidget(codeEditor);

  layout->addWidget(splitter);

  layout->setMargin(0);
  setLayout(layout);

  // Prefer resizing the code editor over the moments editor
  splitter->setStretchFactor(0, 0);
  splitter->setStretchFactor(1, 1);

  // Tell frankensteined widget to resize to proper size
  resize(codeEditor->geometry().width() + momentWidget->geometry().width(), codeEditor->geometry().height());

  for (int moment = 0; moment < momentsModel->rowCount(); ++moment) {
    BindMomentEditor(moment);
  }

  ui->momentsList->setModel(momentsModel);
  ui->momentsList->setModelColumn(Timeline::Moment::kStepFieldNumber);

  if (momentsModel->rowCount() == 0) codeEditor->setDisabled(true);

  connect(ui->momentsList, &QAbstractItemView::clicked, [=](const QModelIndex &index) {
    SetCurrentEditor(index.row());
    ui->stepBox->setValue(momentsModel->data(index.row(), Timeline::Moment::kStepFieldNumber).toInt());
  });

  connect(ui->addMomentButton, &QPushButton::pressed, [=]() {
    AddMoment(ui->stepBox->value());
    codeEditor->setDisabled(false);
    SetCurrentEditor(momentsModel->rowCount()-1);
    ui->stepBox->setValue(ui->stepBox->value() + 1);
  });

  connect(ui->stepBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
    CheckDisableButtons(value);
  });

  connect(ui->changeMomentButton, &QPushButton::pressed, [=]() {
    TimelineChangeMoment dialog(this);
    dialog.setWindowTitle((QString)("Change moment " + QString::number(ui->stepBox->value()) + " to     "));
    dialog.SetSpinBoxValue(ui->stepBox->value());
    if (dialog.exec())
      ChangeMoment(IndexOf(ui->stepBox->value()), dialog.GetSpinBoxValue());

    ui->changeMomentButton->setDown(false); // Qt buggy af
  });

  connect(ui->deleteMomentButton, &QPushButton::pressed, [=]() {
    RemoveMoment(IndexOf(ui->stepBox->value()));

    ui->momentsList->reset();

    if (momentsModel->rowCount() == 0) {
      CheckDisableButtons(-1);
      codeEditor->setDisabled(true);
    } else SetCurrentEditor(momentsModel->rowCount()-1);
  });

  CheckDisableButtons(ui->stepBox->value());
}

TimelineEditor::~TimelineEditor() { delete ui; }

void TimelineEditor::AddMoment(int step) {
  int insertIndex = FindInsertIndex(step);

  momentsModel->insertRow(insertIndex);
  momentsModel->setData(insertIndex, Timeline::Moment::kStepFieldNumber, step);
}

void TimelineEditor::ChangeMoment(int oldIndex, int step) {
  momentsModel->setData(oldIndex, Timeline::Moment::kStepFieldNumber, step);
  momentsModel->moveRows(oldIndex, 1, FindInsertIndex(step));
}

void TimelineEditor::RemoveMoment(int modelIndex) {
 RepeatedProtoModel::RowRemovalOperation remover(momentsModel);
 remover.RemoveRow(modelIndex);
 codeEditor->RemoveCodeWidget(modelIndex);
}

int TimelineEditor::FindInsertIndex(int step) {
  int index = 0;
  while (index < momentsModel->rowCount() && step > momentsModel->
        data(index, Timeline::Moment::kStepFieldNumber).toInt()) {
    ++index;
  }

  return index;
}

int TimelineEditor::IndexOf(int step) {
  int index = -1;
  for (int r = 0; r < momentsModel->rowCount(); ++r) {
    if (momentsModel->data(r, Timeline::Moment::kStepFieldNumber).toInt() == step) {
      index = r;
      break;
    }
  }
  return index;
}

void TimelineEditor::BindMomentEditor(int modelIndex) {
  CodeWidget* codeWidget = codeEditor->AddCodeWidget();
  ModelMapper* mapper(new ModelMapper(momentsModel->GetSubModel(modelIndex), this));
  mapper->addMapping(codeWidget, Timeline::Moment::kCodeFieldNumber);
  mapper->toFirst();
}

void TimelineEditor::SetCurrentEditor(int modelIndex) {
  codeEditor->SetCurrentIndex(modelIndex);
  ui->momentsList->selectionModel()->select(
              momentsModel->index(modelIndex, Timeline::Moment::kStepFieldNumber),
              QItemSelectionModel::QItemSelectionModel::ClearAndSelect);
}

void TimelineEditor::CheckDisableButtons(int value) {
  for (int i = 0; i < momentsModel->rowCount(); ++i) {
    if (momentsModel->data(i, Timeline::Moment::kStepFieldNumber).toInt() == value) {
      ui->addMomentButton->setDisabled(true);
      ui->changeMomentButton->setDisabled(false);
      ui->deleteMomentButton->setDisabled(false);
      return;
    }
  }
  ui->addMomentButton->setDisabled(false);
  ui->changeMomentButton->setDisabled(true);
  ui->deleteMomentButton->setDisabled(true);
}
