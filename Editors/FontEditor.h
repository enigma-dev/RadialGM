#ifndef FONTEDITOR_H
#define FONTEDITOR_H

#include "BaseEditor.h"

namespace Ui {
class FontEditor;
}

class FontEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit FontEditor(QWidget *parent, ProtoModel *model);
  ~FontEditor();

 private:
  Ui::FontEditor *ui;
};

#endif  // FONTEDITOR_H
