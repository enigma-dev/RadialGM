#include "BackgroundEditor.h"
#include "ui_BackgroundEditor.h"
#include "ResourceModel.h"

#include <QDataWidgetMapper>

#include <QDebug>

// this Background class will be flatc codegen from --gen-object-api:
struct Background {
	enum {
		VT_TILE_WIDTH,
		VT_TILE_HEIGHT,
		VT_NAME
	};
	std::string name = "hello";
	int tileWidth = 32;
	int tileHeight = 32;
};

union BackgroundFieldAddress {
	int (Background::*int_field);
	std::string (Background::*str_field);
};

BackgroundEditor::BackgroundEditor(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::BackgroundEditor)
{
	ui->setupUi(this);
	QGraphicsScene* scene = new QGraphicsScene(this);
	QPixmap avatar("C:/Users/Owner/Desktop/bg_intro.png");
	scene->addPixmap(avatar);
	ui->imagePreview->setScene(scene);

	static Background* background = new Background(); // just here for me to test

	// field offset, field address, field type
	static FieldMap fieldMap({
		{ Background::VT_TILE_WIDTH, { &background->tileHeight } },
		{ Background::VT_TILE_HEIGHT, { &background->tileHeight } }
	});

	static ResourceModel* model = new ResourceModel(fieldMap);

	QDataWidgetMapper* mapper = new QDataWidgetMapper(this);
	mapper->setOrientation(Qt::Vertical);
	mapper->setModel(model);

	mapper->addMapping(ui->tileWidthSpinBox, Background::VT_TILE_WIDTH);
	mapper->addMapping(ui->tileHeightSpinBox, Background::VT_TILE_HEIGHT);
	mapper->toFirst();
}

BackgroundEditor::~BackgroundEditor()
{
	delete ui;
}
