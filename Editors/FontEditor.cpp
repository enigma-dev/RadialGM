#include "FontEditor.h"
#include "ui_FontEditor.h"

FontEditor::FontEditor(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::FontEditor)
{
	ui->setupUi(this);
}

FontEditor::~FontEditor()
{
	delete ui;
}
