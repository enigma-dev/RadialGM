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

	ImmediateDataWidgetMapper* mapper = new ImmediateDataWidgetMapper(this);
	mapper->setOrientation(Qt::Vertical);
	auto *mainWindow = static_cast<MainWindow*>(parent);
	mapper->setModel(mainWindow->dataModel());

	mapper->addMapping(ui->smoothCheckBox, Background::kSmoothEdgesFieldNumber);
	mapper->addMapping(ui->preloadCheckBox, Background::kPreloadFieldNumber);
	mapper->addMapping(ui->transparentCheckBox, Background::kTransparentFieldNumber);
	mapper->addMapping(ui->tilesetGroupBox, Background::kUseAsTilesetFieldNumber);
	mapper->addMapping(ui->tileWidthSpinBox, Background::kTileWidthFieldNumber);
	mapper->addMapping(ui->tileHeightSpinBox, Background::kTileHeightFieldNumber);
	mapper->toFirst();
}

BackgroundEditor::~BackgroundEditor()
{
	delete ui;
}

void BackgroundEditor::on_actionSave_triggered()
{
	ui->smoothCheckBox->setAcceptDrops(!ui->smoothCheckBox->acceptDrops());
}
