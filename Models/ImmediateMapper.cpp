#include "ImmediateMapper.h"

#include "ProtoModel.h"

#include <QItemDelegate>
#include <QMetaObject>
#include <QMetaProperty>

ImmediateDataWidgetMapper::ImmediateDataWidgetMapper(QObject *parent) : QDataWidgetMapper(parent) {
  // we do manual submission because auto submission will commit even on left click
  // and give false-positives about the dirty state of our model
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

  // temporarily remove the mapping to this widget while we
  // synchronize the model to the widget's current state
  // this avoids causing the widget to be set again
  auto mappedSection = this->mappedSection(editor);
  auto propertyName = this->mappedPropertyName(editor);
  this->removeMapping(editor);

  delegate->setModelData(editor, model, this->indexAt(mappedSection));

  this->addMapping(editor, mappedSection, propertyName);
}

void ImmediateDataWidgetMapper::removeMapping(QWidget *widget) {
  // remove the connection to the widget's property change meta signal
  auto propertyName = this->mappedPropertyName(widget);
  auto widgetMetaObject = widget->metaObject();
  auto property = widgetMetaObject->property(widgetMetaObject->indexOfProperty(propertyName));
  auto metaObject = this->metaObject();
  QMetaObject::disconnect(widget, property.notifySignalIndex(), this, metaObject->indexOfSlot("widgetChanged()"));

  // remove the internal widget mapper's event filters and such
  QDataWidgetMapper::removeMapping(widget);
  // disown this widget
  this->_widgetList.removeOne(widget);
}

void ImmediateDataWidgetMapper::addMapping(QWidget *widget, int section, QByteArray propertyName) {
  // since QDataWidgetMapper makes its widget map private we have no way of
  // getting all of the widgets registered with this mapper
  // we also do not want to hook up a widget that's already mapped
  if (_widgetList.contains(widget)) return;
  _widgetList.append(widget);
  QDataWidgetMapper::addMapping(widget, section, propertyName);
  // we use the QDataWidgetMapper to tell us what property we should listen to for changes
  // all widgets have several properties, but most have a "primary" property with which
  // the user interacts, i.e. "checked" for QCheckBox
  propertyName = this->mappedPropertyName(widget);
  // we use the widget's meta object in order to get us a signal that we can listen to
  // so we know when this property changes
  auto widgetMetaObject = widget->metaObject();
  auto property = widgetMetaObject->property(widgetMetaObject->indexOfProperty(propertyName));
  auto metaObject = this->metaObject();
  QMetaObject::connect(widget, property.notifySignalIndex(), this, metaObject->indexOfSlot("widgetChanged()"));
}

void ImmediateDataWidgetMapper::clearMapping() {
  // we have to use our own remove mapping since it does not override
  // to ensure we are disconnected from the meta signal for the property change
  foreach (auto widget, _widgetList)
    this->removeMapping(widget);
  QDataWidgetMapper::clearMapping();
}

void ImmediateDataWidgetMapper::setCurrentIndex(int index) {
  // when the current index changes all the widgets have to be reloaded from the model
  // we do not want them to signal property changes to us while they are reloading
  // so we have to block their signals until the current index is changed and then
  // unblock them afterwards if they were not blocked when this method entered
  // NOTE: this probably is not threadsafe or reentrant, just like the rest of Qt
  // and any other GUI framework, so take care when synchronizing it with other threads
  QList<QWidget *> wasBlocked;
  for (QWidget *widget : qAsConst(_widgetList)) {
    if (!widget->blockSignals(true)) {
      wasBlocked.append(widget);
    }
  }
  QDataWidgetMapper::setCurrentIndex(index);
  for (QWidget *widget : wasBlocked) widget->blockSignals(false);
}
