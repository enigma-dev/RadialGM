#ifndef TIMELINEEDITOR_H
#define TIMELINEEDITOR_H

#include "BaseEditor.h"
#include "CodeEditor.h"

namespace Ui {
class TimelineEditor;
class CodeEditor;
}  // namespace Ui

class TimelineEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit TimelineEditor(ProtoModel* model, const QPersistentModelIndex& root, QWidget* parent);
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

  CodeEditor* _codeEditor;
  Ui::TimelineEditor* _ui;
  ProtoModel* _timelineModel;
};

#endif  // TIMELINEEDITOR_H
