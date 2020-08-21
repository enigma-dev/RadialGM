#ifndef OBJECTEDITOR_H
#define OBJECTEDITOR_H

#include "BaseEditor.h"
#include "CodeEditor.h"
#include "Object.pb.h"
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
  void CheckDisableButtons(int value);
  void AddEvent(Object::EgmEvent event);
  void ChangeEvent(int idx, Object::EgmEvent event);
  void RemoveEvent(int idx);
  int IndexOf(Object::EgmEvent event);
  void BindEventEditor(int idx);
  void SetCurrentEditor(int idx);

  Ui::ObjectEditor* _ui;
  MessageModel* _objectModel = nullptr;
  EventsListModel* _eventsModel = nullptr;
  CodeEditor* _codeEditor;
};

#endif  // OBJECTEDITOR_H
