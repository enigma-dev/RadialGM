#ifndef TIMELINEEDITOR_H
#define TIMELINEEDITOR_H

#include "BaseEditor.h"
#include "CodeEditor.h"

namespace Ui {
class TimelineEditor;
class CodeEditor;
}

class TimelineEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit TimelineEditor(ProtoModelPtr model, QWidget* parent);
  ~TimelineEditor();

private:
  void CheckDisableButtons(int value);
  void AddMoment(int step);
  void ChangeMoment(int oldIndex, int step);
  void RemoveMoment(int modelIndex);
  int FindInsertIndex(int step);
  int IndexOf(int step);
  void BindMomentEditor(int modelIndex);
  void SetCurrentEditor(int modelIndex);

  CodeEditor* codeEditor;
  Ui::TimelineEditor* ui;
  RepeatedProtoModelPtr momentsModel;
};

#endif  // TIMELINEEDITOR_H
