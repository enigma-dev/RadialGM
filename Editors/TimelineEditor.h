#ifndef TIMELINEEDITOR_H
#define TIMELINEEDITOR_H

#include "BaseEditor.h"

namespace Ui {
class TimelineEditor;
}

class TimelineEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit TimelineEditor(ProtoModel* model, QWidget* parent);
  ~TimelineEditor();

 private:
  Ui::TimelineEditor* ui;
};

#endif  // TIMELINEEDITOR_H
