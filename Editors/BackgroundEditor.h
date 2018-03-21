#ifndef BACKGROUNDEDITOR_H
#define BACKGROUNDEDITOR_H

#include "Models/ResourceModel.h"

#include "Widgets/BackgroundRenderer.h"

#include <QDataWidgetMapper>
#include <QItemDelegate>
#include <QMetaObject>
#include <QMetaProperty>

#include <QDebug>

class ImmediateDataWidgetMapper : public QDataWidgetMapper {
  Q_OBJECT
 private slots:
  void widgetChanged() {
	auto delegate = this->itemDelegate();
	auto editor = static_cast<QWidget *>(this->sender());
	delegate->commitData(editor);
	delegate->closeEditor(editor, QAbstractItemDelegate::SubmitModelCache);
  }

 public:
  ImmediateDataWidgetMapper(QObject *parent = 0) : QDataWidgetMapper(parent) {}

  void addMapping(QWidget *widget, int section, QByteArray propertyName = "") {
	QDataWidgetMapper::addMapping(widget, section, propertyName);
	propertyName = this->mappedPropertyName(widget);
	auto widgetMetaObject = widget->metaObject();
	auto property = widgetMetaObject->property(widgetMetaObject->indexOfProperty(propertyName));
	auto metaObject = this->metaObject();
	QMetaObject::connect(widget, property.notifySignalIndex(), this, metaObject->indexOfSlot("widgetChanged()"));
  }
};

namespace Ui {
class BackgroundEditor;
}

class BackgroundEditor : public QWidget {
  Q_OBJECT

 public:
  explicit BackgroundEditor(QWidget *parent, ResourceModel *model);
  ~BackgroundEditor();

 private slots:
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
				   const QVector<int> &roles = QVector<int>());
  void on_actionSave_triggered();
  void on_actionZoomIn_triggered();
  void on_actionZoomOut_triggered();
  void on_actionZoom_triggered();

  void on_actionNewImage_triggered();

 private:
  Ui::BackgroundEditor *ui;
  BackgroundRenderer *renderer;
};

#endif  // BACKGROUNDEDITOR_H
