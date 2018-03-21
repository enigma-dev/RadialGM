#ifndef FONTEDITOR_H
#define FONTEDITOR_H

#include "Models/ResourceModel.h"

#include <QWidget>

namespace Ui {
class FontEditor;
}

class FontEditor : public QWidget {
  Q_OBJECT

 public:
  explicit FontEditor(QWidget *parent, ResourceModel *model);
  ~FontEditor();

 private:
  Ui::FontEditor *ui;
};

#endif  // FONTEDITOR_H
