#include "BackgroundEditor.h"
#include "ui_BackgroundEditor.h"
#include "ResourceModel.h"

#include "resources/Background.pb.h"

#include "MainWindow.h"

#include <QPainter>

using buffers::resources::Background;

BackgroundEditor::BackgroundEditor(QWidget *parent, Background *bkg) :
	QWidget(parent),
	ui(new Ui::BackgroundEditor)
{
	ui->setupUi(this);

    ImmediateDataWidgetMapper* mapper = new ImmediateDataWidgetMapper(this);
	mapper->setOrientation(Qt::Vertical);
	static ResourceModel* rm = new ResourceModel(bkg);
	connect(rm, &ResourceModel::dataChanged, this, &BackgroundEditor::dataChanged);
    mapper->setModel(rm);

	mapper->addMapping(ui->smoothCheckBox, Background::kSmoothEdgesFieldNumber);
    mapper->addMapping(ui->preloadCheckBox, Background::kPreloadFieldNumber);
	mapper->addMapping(ui->transparentCheckBox, Background::kTransparentFieldNumber);
	mapper->addMapping(ui->tilesetGroupBox, Background::kUseAsTilesetFieldNumber);
	mapper->addMapping(ui->tileWidthSpinBox, Background::kTileWidthFieldNumber);
	mapper->addMapping(ui->tileHeightSpinBox, Background::kTileHeightFieldNumber);
    mapper->addMapping(ui->horizontalOffsetSpinBox, Background::kHorizontalOffsetFieldNumber);
    mapper->addMapping(ui->verticalOffsetSpinBox, Background::kVerticalOffsetFieldNumber);
    mapper->addMapping(ui->horizontalSpacingSpinBox, Background::kHorizontalSpacingFieldNumber);
    mapper->addMapping(ui->verticalSpacingSpinBox, Background::kVerticalSpacingFieldNumber);
    mapper->toFirst();

    ui->backgroundRenderer->setImage(QString::fromStdString(bkg->image()));
    ui->backgroundRenderer->setGrid(ui->tilesetGroupBox->isChecked(),
                              ui->horizontalOffsetSpinBox->value(),
                              ui->verticalOffsetSpinBox->value(),
                              ui->tileWidthSpinBox->value(),
                              ui->tileHeightSpinBox->value(),
                              ui->horizontalSpacingSpinBox->value(),
                              ui->verticalSpacingSpinBox->value());
}

BackgroundEditor::~BackgroundEditor()
{
	delete ui;
}

void BackgroundEditor::dataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/, const QVector<int>& /*roles*/) {
    ui->backgroundRenderer->setGrid(ui->tilesetGroupBox->isChecked(),
                              ui->horizontalOffsetSpinBox->value(),
                              ui->verticalOffsetSpinBox->value(),
                              ui->tileWidthSpinBox->value(),
                              ui->tileHeightSpinBox->value(),
                              ui->horizontalSpacingSpinBox->value(),
                              ui->verticalSpacingSpinBox->value());
}

void BackgroundEditor::on_actionSave_triggered()
{
	ui->smoothCheckBox->setAcceptDrops(!ui->smoothCheckBox->acceptDrops());
}
