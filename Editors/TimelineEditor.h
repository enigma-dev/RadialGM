#ifndef TIMELINEEDITOR_H
#define TIMELINEEDITOR_H

#include "Models/ResourceModel.h"

#include <QWidget>

namespace Ui {
class TimelineEditor;
}

class TimelineEditor : public QWidget {
  Q_OBJECT

 public:
  explicit TimelineEditor(QWidget *parent, ResourceModel *model);
  ~TimelineEditor();

 private:
  Ui::TimelineEditor *ui;
};

#endif  // TIMELINEEDITOR_H
