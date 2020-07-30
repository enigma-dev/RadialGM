#include "TimelineEditor.h"

#include "ui_CodeEditor.h"
#include "ui_TimelineEditor.h"

#include "CodeEditor.h"
#include "Dialogs/TimelineChangeMoment.h"

#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
#include <QSizePolicy>
#include <QSplitter>

TimelineEditor::TimelineEditor(ProtoModel* model, const QPersistentModelIndex& root, QWidget* parent)
    : BaseEditor(model, root, parent), _codeEditor(new CodeEditor(this, true)), _ui(new Ui::TimelineEditor) {
  QLayout* layout = new QVBoxLayout(this);
  QSplitter* splitter = new QSplitter(this);

  QWidget* momentWidget = new QWidget(this);
  _ui->setupUi(momentWidget);

  _nodeMapper->addMapping(_ui->nameEdit, TreeNode::kNameFieldNumber);

  connect(_ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);

  splitter->addWidget(momentWidget);
  splitter->addWidget(_codeEditor);

  layout->addWidget(splitter);

  layout->setMargin(0);
  setLayout(layout);

  // Prefer resizing the code editor over the moments editor
  splitter->setStretchFactor(0, 0);
  splitter->setStretchFactor(1, 1);

  // Tell frankensteined widget to resize to proper size
  resize(_codeEditor->geometry().width() + momentWidget->geometry().width(), _codeEditor->geometry().height());

  connect(_ui->stepBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) { CheckDisableButtons(value); });

  connect(_ui->changeMomentButton, &QPushButton::pressed, [=]() {
    TimelineChangeMoment dialog(this);
    dialog.setWindowTitle((QString)("Change moment " + QString::number(_ui->stepBox->value()) + " to     "));
    dialog.SetSpinBoxValue(_ui->stepBox->value());
    if (dialog.exec()) ChangeMoment(IndexOf(_ui->stepBox->value()), dialog.GetSpinBoxValue());

    _ui->momentsList->reset();

    _ui->changeMomentButton->setDown(false);  // Qt buggy af
  });
}

TimelineEditor::~TimelineEditor() { delete _ui; }

void TimelineEditor::RebindSubModels() {
  _codeEditor->ClearCodeWidgets();



}

void TimelineEditor::AddMoment(int step) {

}

void TimelineEditor::ChangeMoment(int oldIndex, int step) {

}

void TimelineEditor::RemoveMoment(int modelIndex) {

}

int TimelineEditor::FindInsertIndex(int step) {
  int index = 0;

  return index;
}

int TimelineEditor::IndexOf(int step) {


  return -1;
}

void TimelineEditor::BindMomentEditor(int modelIndex) {

}

void TimelineEditor::SetCurrentEditor(int modelIndex) {

}

void TimelineEditor::CheckDisableButtons(int value) {

}
