#include "ImmediateMapper.h"

#include "ProtoModel.h"

#include <QItemDelegate>
#include <QMetaObject>
#include <QMetaProperty>

ImmediateDataWidgetMapper::ImmediateDataWidgetMapper(QObject *parent) : QDataWidgetMapper(parent) {
  this->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
}

QModelIndex ImmediateDataWidgetMapper::indexAt(int section) {
  auto currentIndex = this->currentIndex();
  auto model = this->model();
  auto rootIndex = this->rootIndex();
  return (this->orientation() == Qt::Horizontal) ? model->index(currentIndex, section, rootIndex)
                                                 : model->index(section, currentIndex, rootIndex);
}

void ImmediateDataWidgetMapper::widgetChanged() {
  auto delegate = this->itemDelegate();
  auto editor = static_cast<QWidget *>(this->sender());
  auto model = this->model();
  delegate->setModelData(editor, model, this->indexAt(this->mappedSection(editor)));
}

void ImmediateDataWidgetMapper::addMapping(QWidget *widget, int section, QByteArray propertyName) {
  if (widgetList.contains(widget)) return;
  widgetList.append(widget);
  QDataWidgetMapper::addMapping(widget, section, propertyName);
  propertyName = this->mappedPropertyName(widget);
  auto widgetMetaObject = widget->metaObject();
  auto property = widgetMetaObject->property(widgetMetaObject->indexOfProperty(propertyName));
  auto metaObject = this->metaObject();
  QMetaObject::connect(widget, property.notifySignalIndex(), this, metaObject->indexOfSlot("widgetChanged()"));
}

void ImmediateDataWidgetMapper::clearMapping() {
  widgetList.clear();
  QDataWidgetMapper::clearMapping();
}

void ImmediateDataWidgetMapper::setCurrentIndex(int index) {
  QList<QWidget *> wasBlocked;
  for (QWidget *widget : widgetList) {
    if (!widget->blockSignals(true)) {
      wasBlocked.append(widget);
    }
  }
  QDataWidgetMapper::setCurrentIndex(index);
  for (QWidget *widget : wasBlocked) widget->blockSignals(false);
}
