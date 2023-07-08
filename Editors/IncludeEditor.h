#ifndef INCLUDEEDITOR_H
#define INCLUDEEDITOR_H

#include <QWidget>

namespace Ui {
class IncludeEditor;
}

class IncludeEditor : public QWidget
{
  Q_OBJECT

public:
  explicit IncludeEditor(QWidget *parent = nullptr);
  ~IncludeEditor();

private:
  Ui::IncludeEditor *ui;
};

#endif // INCLUDEEDITOR_H
