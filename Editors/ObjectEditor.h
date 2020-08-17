#ifndef OBJECTEDITOR_H
#define OBJECTEDITOR_H

#include "BaseEditor.h"

#include "Models/EventsListModel.h"

namespace Ui {
class ObjectEditor;
}

class ObjectEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit ObjectEditor(MessageModel* model, QWidget* parent);
  ~ObjectEditor() override;

 public slots:
  void RebindSubModels() override;

 private:
  Ui::ObjectEditor* _ui;
  MessageModel* _objectModel = nullptr;
  EventsListModel* _eventsModel = nullptr;
};

#endif  // OBJECTEDITOR_H
