#include "ImmediateMapper.h"

#include "ProtoModel.h"

#include <QItemDelegate>
#include <QMetaObject>
#include <QMetaProperty>

ImmediateDataWidgetMapper::ImmediateDataWidgetMapper(QObject *parent) : QDataWidgetMapper(parent) {}

void ImmediateDataWidgetMapper::widgetChanged() {
  auto delegate = this->itemDelegate();
  auto editor = static_cast<QWidget *>(this->sender());
  delegate->commitData(editor);
  delegate->closeEditor(editor, QAbstractItemDelegate::SubmitModelCache);
}

void ImmediateDataWidgetMapper::addMapping(QWidget *widget, int section, QByteArray propertyName) {
  QDataWidgetMapper::addMapping(widget, section, propertyName);
  propertyName = this->mappedPropertyName(widget);
  auto widgetMetaObject = widget->metaObject();
  auto property = widgetMetaObject->property(widgetMetaObject->indexOfProperty(propertyName));
  auto metaObject = this->metaObject();
  QMetaObject::connect(widget, property.notifySignalIndex(), this, metaObject->indexOfSlot("widgetChanged()"));
}

void ImmediateDataWidgetMapper::toFirst() {
  QDataWidgetMapper::toFirst();
  // Mapper::toFirst triggers SetData and marks our model as dirty when it's really a virgin
  static_cast<ProtoModel *>(model())->SetDirty(false);
}
