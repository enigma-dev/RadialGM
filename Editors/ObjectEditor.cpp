#include "ObjectEditor.h"
#include "ui_ObjectEditor.h"

ObjectEditor::ObjectEditor(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ObjectEditor)
{
	ui->setupUi(this);
}

ObjectEditor::~ObjectEditor()
{
	delete ui;
}
