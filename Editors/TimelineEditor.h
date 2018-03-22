#ifndef TIMELINEEDITOR_H
#define TIMELINEEDITOR_H

#include "BaseEdtior.h"

namespace Ui {
class TimelineEditor;
}

class TimelineEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit TimelineEditor(QWidget *parent, ResourceModel *model);
  ~TimelineEditor();

 private:
  Ui::TimelineEditor *ui;
};

#endif  // TIMELINEEDITOR_H
