#ifndef OBJECTEDITOR_H
#define OBJECTEDITOR_H

#include "BaseEditor.h"
#include "Object.pb.h"
#include "Models/EventsListModel.h"
#include "Models/EventTypesListModel.h"

#include <QSortFilterProxyModel>
#include <QToolButton>

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

private slots:
  void on_spriteButton_triggered(QAction *arg1);

private:
  void BindEventMenu(QToolButton* btn, bool add);
  void AddChangeFromMenuEvent(const QModelIndex &index, bool add);
  void AddChangeEventHelper(const Object::EgmEvent& event, bool add);
  void CheckDisableButtons();
  void AddEvent(Object::EgmEvent event);
  void ChangeEvent(int idx, Object::EgmEvent event, bool changeCode = true);
  void RemoveEvent(int idx);
  int IndexOf(Object::EgmEvent event);
  void BindEventEditor(int idx);
  void SetCurrentEditor(int idx);
  int MapRowTo(int row);
  int MapRowFrom(int row);

  Ui::ObjectEditor* _ui;
  MessageModel* _objectModel = nullptr;
  EventsListModel* _eventsModel = nullptr;
  QSortFilterProxyModel* _sortedEvents;
  EventTypesListSortFilterProxyModel* _eventsTypesModel;
};

#endif  // OBJECTEDITOR_H
