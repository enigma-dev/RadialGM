#include "BackgroundEditor.h"
#include "ui_BackgroundEditor.h"
#include "ResourceModel.h"

#include "resources/Background.pb.h"

#include "MainWindow.h"

#include <QDataWidgetMapper>
#include <QStyledItemDelegate>
#include <QItemDelegate>
#include <QItemEditorFactory>
#include <QDebug>

class PropertyChangeEventFilter : public QObject
{
public:
	PropertyChangeEventFilter(QObject *parent = 0): QObject(parent) {}

	bool eventFilter(QObject *object, QEvent *event);
};

class ImmediateDataWidgetMapper : public QDataWidgetMapper
{
public:
	ImmediateDataWidgetMapper(QWidget *parent = 0): QDataWidgetMapper(parent) {}

	void addMapping(QWidget *widget, int section, const QByteArray &propertyName = "") {
		QDataWidgetMapper::addMapping(widget, section, propertyName);
		widget->installEventFilter(new PropertyChangeEventFilter(this));
	}
};

bool PropertyChangeEventFilter::eventFilter(QObject *object, QEvent *event) {
	if (event->type() == QEvent::DynamicPropertyChange) {
		//auto *propertyChangeEvent = static_cast<QDynamicPropertyChangeEvent *>(event);
		auto *mapper = reinterpret_cast<ImmediateDataWidgetMapper *>(parent());
		auto *widget = reinterpret_cast<QWidget *>(object);
		//qDebug() << propertyChangeEvent->propertyName();
		mapper->itemDelegate()->commitData(widget);
		mapper->itemDelegate()->closeEditor(widget, QAbstractItemDelegate::SubmitModelCache);
		return true;
	}
	return false;
}

using buffers::resources::Background;

BackgroundEditor::BackgroundEditor(QWidget *parent, Background *bkg) :
	QWidget(parent),
	ui(new Ui::BackgroundEditor)
{
	ui->setupUi(this);

    scene = new QGraphicsScene(this);
    image = QPixmap(QString::fromStdString(bkg->image()));

    ImmediateDataWidgetMapper* mapper = new ImmediateDataWidgetMapper(this);
	mapper->setOrientation(Qt::Vertical);
    ResourceModel* rm = new ResourceModel(bkg);
    connect(rm, &ResourceModel::dataChanged, this, &BackgroundEditor::on_model_modified);
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

    draw();
}

BackgroundEditor::~BackgroundEditor()
{
	delete ui;
}

void BackgroundEditor::draw() {
    scene->clear();
    scene->addPixmap(image);

    if (ui->tilesetGroupBox->isChecked()) {
        unsigned hOff = ui->horizontalOffsetSpinBox->value();
        unsigned vOff = ui->horizontalOffsetSpinBox->value();
        unsigned w = ui->tileWidthSpinBox->value();
        unsigned h = ui->tileHeightSpinBox->value();
        unsigned hSpacing = ui->horizontalSpacingSpinBox->value();
        unsigned vSpacing = ui->verticalSpacingSpinBox->value();

        for (int x = hOff; x < image.width(); x+= w + hSpacing) {
            for (int y = vOff; y < image.height(); y+= h + vSpacing) {
                scene->addRect(x, y, w, h);
            }
        }
    }

    ui->imagePreview->setScene(scene);
}

void BackgroundEditor::on_model_modified() {
    draw();
}

void BackgroundEditor::on_actionSave_triggered()
{
	ui->smoothCheckBox->setAcceptDrops(!ui->smoothCheckBox->acceptDrops());
}
