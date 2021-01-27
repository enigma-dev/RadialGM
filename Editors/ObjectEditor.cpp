#include "ObjectEditor.h"
#include "Components/QMenuView.h"
#include "Dialogs/EventArgumentsDialog.h"
#include "MainWindow.h"
#include "Models/EventTypesListModel.h"
#include "Models/RepeatedMessageModel.h"
#include "Models/RepeatedModel.h"

#include "ui_ObjectEditor.h"

#include <QSplitter>

void ObjectEditor::BindEventMenu(QToolButton *btn, bool add) {
  QMenuView *eventsMenu = new QMenuView(this);
  eventsMenu->setModel(_eventsTypesModel);
  eventsMenu->setToolTipsVisible(true);
  btn->setMenu(eventsMenu);
  connect(eventsMenu, &QMenuView::triggered, [=](const QModelIndex &index) { AddChangeFromMenuEvent(index, add); });
}

ObjectEditor::ObjectEditor(MessageModel *model, QWidget *parent)
    : BaseEditor(model, parent), _ui(new Ui::ObjectEditor) {
  _ui->setupUi(this);

  _nodeMapper->addMapping(_ui->nameEdit, TreeNode::kNameFieldNumber);

  connect(_ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);

  _eventsModel = new EventsListModel(MainWindow::GetEventData(), this);

  EventTypesListModel *m = new EventTypesListModel(MainWindow::GetEventData(), this);
  _eventsTypesModel = new EventTypesListSortFilterProxyModel(m);
  _eventsTypesModel->setSourceModel(m);
  _eventsTypesModel->sort(0);

  BindEventMenu(_ui->addEventButton, true);
  BindEventMenu(_ui->changeEventButton, false);

  RebindSubModels();
}

ObjectEditor::~ObjectEditor() { delete _ui; }

void ObjectEditor::AddChangeEventHelper(const Object::EgmEvent &event, bool add) {
  if (add) {
    AddEvent(event);
  } else {
    int existingIdx = MapRowFrom(IndexOf(event));
    int selectedIdx = MapRowTo(_ui->eventsList->selectionModel()->currentIndex().row());
    if (existingIdx == -1 && selectedIdx != -1) {
      ChangeEvent(selectedIdx, event, false);
    } else
      qDebug() << "Change event failed";
  }
}

void ObjectEditor::AddChangeFromMenuEvent(const QModelIndex &index, bool add) {
  QStringList args = _eventsTypesModel->data(index, EventTypesListModel::UserRoles::EventArgumentsRole).toStringList();
  EventArgumentsDialog *dialog = nullptr;
  if (args.size() > 0) {
    dialog = new EventArgumentsDialog(this, args);
    dialog->open();

    connect(dialog, &QDialog::accepted, [=]() {
      if (dialog->result() == QDialog::Accepted) {
        Object::EgmEvent event;
        event.set_id(
            _eventsTypesModel->data(index, EventTypesListModel::UserRoles::EventBareIDRole).toString().toStdString());
        for (const QString &arg : dialog->GetArguments()) {
          std::string *s = event.add_arguments();
          s->assign(arg.toStdString());
        }

        AddChangeEventHelper(event, add);
      }
    });
  } else {
    Object::EgmEvent event;
    event.set_id(
        _eventsTypesModel->data(index, EventTypesListModel::UserRoles::EventBareIDRole).toString().toStdString());
    AddChangeEventHelper(event, add);
  }
}

void ObjectEditor::RebindSubModels() {
  _objectModel = _model->GetSubModel<MessageModel *>(TreeNode::kObjectFieldNumber);
  _eventsModel->setSourceModel(_objectModel->GetSubModel<RepeatedMessageModel *>(Object::kEgmEventsFieldNumber));

  _sortedEvents = new QSortFilterProxyModel(_eventsModel);
  _sortedEvents->setSourceModel(_eventsModel);
  _sortedEvents->sort(0);
  _sortedEvents->setDynamicSortFilter(true);
  _ui->eventsList->setModel(_sortedEvents);

  connect(_ui->eventsList, &QAbstractItemView::clicked,
          [=](const QModelIndex &index) { SetCurrentEditor(MapRowTo(index.row())); });

  connect(_ui->deleteEventButton, &QToolButton::pressed, [=]() {
    QModelIndex selection = _ui->eventsList->selectionModel()->currentIndex();
    if (selection.row() != -1) RemoveEvent(MapRowTo(selection.row()));
  });

  for (int event = 0; event < _eventsModel->rowCount(); ++event) {
    BindEventEditor(event);
  }

  SetCurrentEditor(MapRowFrom(0));
  CheckDisableButtons();

  BaseEditor::RebindSubModels();
}

void ObjectEditor::CheckDisableButtons() {
  bool hasEvents = _sortedEvents->rowCount() > 0;
  _ui->changeEventButton->setDisabled(!hasEvents);
  _ui->deleteEventButton->setDisabled(!hasEvents);
  _ui->codeEditor->setDisabled(!hasEvents);
  if (!hasEvents) _ui->eventLineEdit->setText("");
}

void ObjectEditor::AddEvent(Object::EgmEvent event) {
  RepeatedMessageModel *eventsModel = _objectModel->GetSubModel<RepeatedMessageModel *>(Object::kEgmEventsFieldNumber);
  int idx = eventsModel->rowCount();

  if (IndexOf(event) == -1) {
    bool insert = eventsModel->insertRow(idx);
    if (insert) {
      BindEventEditor(idx);
      ChangeEvent(idx, event);
    }
  } else
    qDebug() << "Event already exists";
}

void ObjectEditor::ChangeEvent(int idx, Object::EgmEvent event, bool changeCode) {
  RepeatedMessageModel *eventsModel = _objectModel->GetSubModel<RepeatedMessageModel *>(Object::kEgmEventsFieldNumber);

  eventsModel->SetData(QString::fromStdString(event.id()), idx, Object::EgmEvent::kIdFieldNumber);

  if (changeCode) eventsModel->SetData(QString::fromStdString(event.code()), idx, Object::EgmEvent::kCodeFieldNumber);

  RepeatedStringModel *argsModel = eventsModel->GetSubModel<MessageModel *>(idx)->GetSubModel<RepeatedStringModel *>(
      Object::EgmEvent::kArgumentsFieldNumber);

  size_t argc = 0;
  argsModel->removeRows(0, argsModel->rowCount());  // clear old arguments
  if (event.arguments_size() > 0) {
    argsModel->insertRows(argsModel->rowCount(), event.arguments_size());
    for (const auto &arg : event.arguments()) {
      argsModel->SetData(QString::fromStdString(arg), argc++);
    }
  }

  SetCurrentEditor(idx);
  CheckDisableButtons();
}

void ObjectEditor::RemoveEvent(int idx) {
  RepeatedMessageModel *eventsModel = _objectModel->GetSubModel<RepeatedMessageModel *>(Object::kEgmEventsFieldNumber);
  eventsModel->removeRow(idx);
  _ui->codeEditor->RemoveCodeWidget(idx);
  SetCurrentEditor(MapRowFrom(0));
  CheckDisableButtons();
}

int ObjectEditor::IndexOf(Object::EgmEvent event) {
  std::vector<std::string> args(event.arguments().begin(), event.arguments().end());
  Event e = MainWindow::GetEventData()->get_event(event.id(), args);
  for (int i = 0; i < _eventsModel->rowCount(); ++i) {
    if (_eventsModel->data(_eventsModel->index(i, 0)).toString() == QString::fromStdString(e.HumanName())) return i;
  }
  return -1;
}

void ObjectEditor::BindEventEditor(int idx) {
  RepeatedMessageModel *eventsModel = _objectModel->GetSubModel<RepeatedMessageModel *>(Object::kEgmEventsFieldNumber);
  CodeWidget *codeWidget = _ui->codeEditor->AddCodeWidget();
  ModelMapper *mapper(new ModelMapper(eventsModel->GetSubModel<MessageModel *>(idx), this));
  mapper->addMapping(codeWidget, Object::EgmEvent::kCodeFieldNumber);
  mapper->toFirst();
}

void ObjectEditor::SetCurrentEditor(int idx) {
  if (idx < _sortedEvents->rowCount()) {
    _ui->codeEditor->SetCurrentIndex(idx);
    _ui->eventLineEdit->setText(_eventsModel->data(_eventsModel->index(idx, 0)).toString());
    _ui->eventsList->selectionModel()->select(_sortedEvents->index(MapRowFrom(idx), 0),
                                              QItemSelectionModel::QItemSelectionModel::ClearAndSelect);
  }
}

int ObjectEditor::MapRowTo(int row) { return _sortedEvents->mapToSource(_sortedEvents->index(row, 0)).row(); }

int ObjectEditor::MapRowFrom(int row) { return _sortedEvents->mapFromSource(_eventsModel->index(row, 0)).row(); }
