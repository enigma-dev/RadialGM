#include "PathEditor.h"
#include "ui_PathEditor.h"

PathEditor::PathEditor(QWidget *parent, ResourceModel* /*model*/) :
	QWidget(parent),
	ui(new Ui::PathEditor)
{
	ui->setupUi(this);
}

PathEditor::~PathEditor()
{
	delete ui;
}
