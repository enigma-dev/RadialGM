#ifndef TIMELINEEDITOR_H
#define TIMELINEEDITOR_H

#include "BaseEditor.h"
#include "CodeEditor.h"

#include "Models/RepeatedMessageModel.h"

namespace Ui {
class TimelineEditor;
class CodeEditor;
}  // namespace Ui

class TimelineEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit TimelineEditor(MessageModel* model, QWidget* parent);
  ~TimelineEditor() override;

 public slots:
  void RebindSubModels() override;

 private:
  void CheckDisableButtons(int value);
  void AddMoment(int step);
  void ChangeMoment(int oldIndex, int step);
  void RemoveMoment(int modelIndex);
  int FindInsertIndex(int step);
  int IndexOf(int step);
  void BindMomentEditor(int modelIndex);
  void SetCurrentEditor(int modelIndex);

  Ui::TimelineEditor* _ui;
  MessageModel* _timelineModel;
  RepeatedMessageModel* _momentsModel;
};

#endif  // TIMELINEEDITOR_H
