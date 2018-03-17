#include "BackgroundEditor.h"
#include "ui_BackgroundEditor.h"
#include "ResourceModel.h"

#include <QDataWidgetMapper>

#include <QDebug>

BackgroundEditor::BackgroundEditor(QWidget *parent, buffers::resources::Background* bkg) :
	QWidget(parent),
	ui(new Ui::BackgroundEditor)
{
	using buffers::resources::Background;

	ui->setupUi(this);
	QGraphicsScene* scene = new QGraphicsScene(this);
    QPixmap avatar(QString::fromStdString(bkg->image()));
	scene->addPixmap(avatar);
	ui->imagePreview->setScene(scene);

    ResourceModel* model = new ResourceModel(bkg);

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
