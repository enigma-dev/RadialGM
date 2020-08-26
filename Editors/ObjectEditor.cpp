#include "Models/RepeatedMessageModel.h"
#include "Models/RepeatedStringModel.h"
#include "Models/EventTypesListModel.h"
#include "Dialogs/EventArgumentsDialog.h"
#include "ObjectEditor.h"
#include "MainWindow.h"
#include "Components/QMenuView.h"

#include "ui_ObjectEditor.h"

#include <QSortFilterProxyModel>
#include <QSplitter>

ObjectEditor::ObjectEditor(MessageModel* model, QWidget* parent) : BaseEditor(model, parent),
    _ui(new Ui::ObjectEditor),
    _codeEditor(new CodeEditor(this, true))
{

  QLayout* layout = new QVBoxLayout(this);
  QSplitter* splitter = new QSplitter(this);

  QWidget* eventsWidget = new QWidget(this);
  _ui->setupUi(eventsWidget);

  _nodeMapper->addMapping(_ui->nameEdit, TreeNode::kNameFieldNumber);

  connect(_ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);

  splitter->addWidget(eventsWidget);
  splitter->addWidget(_codeEditor);

  layout->addWidget(splitter);

  layout->setMargin(0);
  setLayout(layout);

  // Prefer resizing the code editor over the moments editor
  splitter->setStretchFactor(0, 0);
  splitter->setStretchFactor(1, 1);

  // Tell frankensteined widget to resize to proper size
  resize(_codeEditor->geometry().width() + eventsWidget->geometry().width(), _codeEditor->geometry().height());

  connect(_ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);

  _eventsModel = new EventsListModel(MainWindow::GetEventData(), this);

  EventTypesListModel* m = new EventTypesListModel(MainWindow::GetEventData(), this);
  QMenuView* eventsMenu = new QMenuView(this);
  EventTypesListSortFilterProxyModel* mm = new EventTypesListSortFilterProxyModel(m);
  mm->setSourceModel(m);
  mm->sort(0);
  eventsMenu->setModel(mm);
  eventsMenu->setToolTipsVisible(true);
  _ui->addEventButton->setMenu(eventsMenu);

  connect(eventsMenu, &QMenuView::triggered, [=](const QModelIndex& index) {
    QStringList args = mm->data(index, Qt::UserRole+2).toStringList();
    EventArgumentsDialog* dialog = nullptr;
    if (args.size() > 0) {
      dialog = new EventArgumentsDialog(this, args);
      dialog->open();

      connect(dialog, &QDialog::accepted, [=]() {
        if (dialog->result() == QDialog::Accepted) {
          Object::EgmEvent event;
          event.set_id(mm->data(index, Qt::UserRole+3).toString().toStdString());
          for (const QString& arg : dialog->GetArguments()) {
            std::string* s = event.add_arguments();
            s->assign(arg.toStdString());
          }

          AddEvent(event);
        }
      });
    } else {
      Object::EgmEvent event;
      event.set_id(mm->data(index, Qt::UserRole+3).toString().toStdString());
      AddEvent(event);
    }
  });

  RebindSubModels();
}

ObjectEditor::~ObjectEditor() { delete _ui; }

void ObjectEditor::RebindSubModels() {
  _objectModel = _model->GetSubModel<MessageModel*>(TreeNode::kObjectFieldNumber);
  _eventsModel->setSourceModel(_objectModel->GetSubModel<RepeatedMessageModel*>(Object::kEgmEventsFieldNumber));

  QSortFilterProxyModel* sortedEvents = new QSortFilterProxyModel(_eventsModel);
  sortedEvents->setSourceModel(_eventsModel);
  sortedEvents->sort(0);
  _ui->eventsList->setModel(sortedEvents);

  connect(_ui->eventsList, &QAbstractItemView::clicked, [=](const QModelIndex& index) {
    SetCurrentEditor(sortedEvents->mapFromSource(_eventsModel->index(index.row(), 0)).row());
  });

  for (int event = 0; event < _eventsModel->rowCount(); ++event) {
    BindEventEditor(event);
  }

  BaseEditor::RebindSubModels();
}

void ObjectEditor::CheckDisableButtons(int value) {

}

void ObjectEditor::AddEvent(Object::EgmEvent event) {
  RepeatedMessageModel* eventsModel = _objectModel->GetSubModel<RepeatedMessageModel*>(Object::kEgmEventsFieldNumber);
  int idx = eventsModel->rowCount();
  bool insert = eventsModel->insertRow(idx);
  if (insert) { ChangeEvent(idx, event); }
}

void ObjectEditor::ChangeEvent(int idx, Object::EgmEvent event) {
  RepeatedMessageModel* eventsModel = _objectModel->GetSubModel<RepeatedMessageModel*>(Object::kEgmEventsFieldNumber);

  eventsModel->SetData(QString::fromStdString(event.id()), idx, Object::EgmEvent::kIdFieldNumber);
  eventsModel->SetData(QString::fromStdString(event.code()), idx, Object::EgmEvent::kCodeFieldNumber);

  RepeatedStringModel* argsModel =
    eventsModel->GetSubModel<MessageModel*>(idx)
      ->GetSubModel<RepeatedStringModel*>(Object::EgmEvent::kArgumentsFieldNumber);

  size_t argc = 0;
  argsModel->removeRows(0, argsModel->rowCount()); // clear old arguments
  if (event.arguments_size() > 0) {
    argsModel->insertRows(argsModel->rowCount(), event.arguments_size());
    for (const auto& arg : event.arguments()) {
      argsModel->SetData(QString::fromStdString(arg), argc++);
    }
  }

  _ui->eventsList->sortByColumn(0, Qt::AscendingOrder);
}

void ObjectEditor::RemoveEvent(int idx) {
  RepeatedMessageModel* eventsModel = _objectModel->GetSubModel<RepeatedMessageModel*>(Object::kEgmEventsFieldNumber);
  eventsModel->removeRow(idx);
}

int ObjectEditor::IndexOf(Object::EgmEvent event) {
  return -1;
}

void ObjectEditor::BindEventEditor(int idx) {
  RepeatedMessageModel* eventsModel = _objectModel->GetSubModel<RepeatedMessageModel*>(Object::kEgmEventsFieldNumber);
  CodeWidget* codeWidget = _codeEditor->AddCodeWidget();
  ModelMapper* mapper(new ModelMapper(eventsModel->GetSubModel<MessageModel*>(idx), this));
  mapper->addMapping(codeWidget, Object::EgmEvent::kCodeFieldNumber);
  mapper->toFirst();
}

void ObjectEditor::SetCurrentEditor(int idx) {
  _codeEditor->SetCurrentIndex(idx);
  /*_ui->momentsList->selectionModel()->select(_momentsModel->index(modelIndex, Timeline::Moment::kStepFieldNumber),
                                             QItemSelectionModel::QItemSelectionModel::ClearAndSelec*/
}
