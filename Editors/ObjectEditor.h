#ifndef OBJECTEDITOR_H
#define OBJECTEDITOR_H

#include "ResourceModel.h"

#include <QWidget>

namespace Ui {
class ObjectEditor;
}

class ObjectEditor : public QWidget
{
	Q_OBJECT

public:
    explicit ObjectEditor(QWidget *parent, ResourceModel *model);
	~ObjectEditor();

private:
	Ui::ObjectEditor *ui;
};

#endif // OBJECTEDITOR_H
