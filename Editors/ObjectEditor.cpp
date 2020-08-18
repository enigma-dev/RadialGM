#include "Models/RepeatedMessageModel.h"
#include "ObjectEditor.h"
#include "MainWindow.h"

#include "ui_ObjectEditor.h"

ObjectEditor::ObjectEditor(MessageModel* model, QWidget* parent) : BaseEditor(model, parent), _ui(new Ui::ObjectEditor) {
  _ui->setupUi(this);
  connect(_ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);

  _eventsModel = new EventsListModel(MainWindow::GetEventData(), this);

  RebindSubModels();
}

ObjectEditor::~ObjectEditor() { delete _ui; }

void ObjectEditor::RebindSubModels() {
  _objectModel = _model->GetSubModel<MessageModel*>(TreeNode::kObjectFieldNumber);
  _eventsModel->setSourceModel(_objectModel->GetSubModel<RepeatedMessageModel*>(Object::kEgmEventsFieldNumber));
  //_eventsModel->sort(0, Qt::DescendingOrder);
  _ui->eventsList->setModel(_eventsModel);

  BaseEditor::RebindSubModels();
}
