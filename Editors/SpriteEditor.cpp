#include "SpriteEditor.h"
#include "ui_SpriteEditor.h"

SpriteEditor::SpriteEditor(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SpriteEditor)
{
	ui->setupUi(this);
}

SpriteEditor::~SpriteEditor()
{
	delete ui;
}
