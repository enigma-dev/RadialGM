#ifndef PATHEDITOR_H
#define PATHEDITOR_H

#include "ResourceModel.h"

#include <QWidget>

namespace Ui {
class PathEditor;
}

class PathEditor : public QWidget
{
	Q_OBJECT

public:
    explicit PathEditor(QWidget *parent, ResourceModel *model);
	~PathEditor();

private:
	Ui::PathEditor *ui;
};

#endif // PATHEDITOR_H
