#ifndef INFORMATIONEDITOR_H
#define INFORMATIONEDITOR_H

#include <QWidget>

namespace Ui {
class InformationEditor;
}

class InformationEditor : public QWidget
{
  Q_OBJECT

public:
  explicit InformationEditor(QWidget *parent = nullptr);
  ~InformationEditor();

private:
  Ui::InformationEditor *ui;
};

#endif // INFORMATIONEDITOR_H
