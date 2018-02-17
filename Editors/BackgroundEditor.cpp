#include "BackgroundEditor.h"
#include "ui_BackgroundEditor.h"
#include "ResourceModel.h"

#include "resources/Background.pb.h"

#include <QDataWidgetMapper>

#include <QDebug>

BackgroundEditor::BackgroundEditor(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::BackgroundEditor)
{
	using buffers::resources::Background;

	ui->setupUi(this);
	QGraphicsScene* scene = new QGraphicsScene(this);
	QPixmap avatar("C:/Users/Owner/Desktop/bg_intro.png");
	scene->addPixmap(avatar);
	ui->imagePreview->setScene(scene);

	static Background* background = new Background(); // just here for me to test

	static ResourceModel* model = new ResourceModel(background);

	QDataWidgetMapper* mapper = new QDataWidgetMapper(this);
	mapper->setOrientation(Qt::Vertical);
	mapper->setModel(model);

	mapper->addMapping(ui->tileWidthSpinBox, Background::kTileWidthFieldNumber);
	mapper->addMapping(ui->tileHeightSpinBox, Background::kTileHeightFieldNumber);
	mapper->toFirst();
}

BackgroundEditor::~BackgroundEditor()
{
	delete ui;
}
