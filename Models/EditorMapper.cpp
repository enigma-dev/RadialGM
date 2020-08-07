#include "Models/EditorMapper.h"
#include "Models/EditorModel.h"

#include "Editors/BaseEditor.h"

#include "Components/Logger.h"

// we don't want BaseEditor & subclasses knowing about tree node
// structure, as that is fragile and slows down compilation!
#include "treenode.pb.h"

#include <QMetaObject>
#include <QMetaProperty>

using TreeNode = buffers::TreeNode;
using MapGroup = EditorMapper::MapGroup;

EditorMapper::EditorMapper(EditorModel *model, BaseEditor *parent) : QObject(parent),
  _model(model), _index(QModelIndex()) {
  // ask the source for a message pointer
  auto ptr = _model->data(QModelIndex(),Qt::UserRole+1).value<void*>();
  auto msg = static_cast<Message*>(ptr);
  // cache the descriptor only
  _rootDesc = msg->GetDescriptor();

  //TODO: Handle row remove and stuff
  connect(_model, &EditorModel::dataChanged, this, [this](
          const QModelIndex& topLeft, const QModelIndex& bottomRight){
    this->load(topLeft, false);
  });
}

void EditorMapper::mapField(int fieldNumber, QObject *object, const QByteArray& property) {
  auto desc = _desc.empty() ? _rootDesc : _desc.top();
  auto field = desc->FindFieldByNumber(fieldNumber);
  R_EXPECT_V(field != nullptr) << "Mapping field" << fieldNumber << "which doesn't exist.";
  auto index = _model->index(field->index(), 0, _index);

  // try to find the property or a default "USER" property
  // https://doc.qt.io/qt-5/qmetaproperty.html#isUser
  auto metaObject = object->metaObject();
  int metaIndex = metaObject->indexOfProperty(property);
  QMetaProperty metaProperty =
      (metaIndex == -1) ?
        metaObject->userProperty() :
        metaObject->property(metaIndex);

  R_EXPECT_V(metaProperty.isValid()) << "Mapped property" << property
                                     << "is not valid on object" << object;

  auto notifySignal = metaProperty.notifySignalIndex();
  auto mapping = _mappings[index];

  R_ASSESS(!mapping.contains(ObjectProperty(object,metaProperty.propertyIndex())))
    << "Object" << object
    << "property" << metaProperty.name()
    << "mapped again to the same model index!";

  QMetaObject::connect(object, notifySignal, this,
    this->metaObject()->indexOfSlot("objectPropertyChanged()"));
  mapping.insert(ObjectProperty(object,metaProperty.propertyIndex()));
}

void EditorMapper::mapName(QObject *object, const QByteArray& property) {
  mapField(TreeNode::kNameFieldNumber, object, property);
}

void EditorMapper::clear() {
  popRoot(); // << back to the root automatically
  for (auto& props : _mappings) {
    for (auto& prop : props) {
      auto object = prop.first;
      auto propIndex = prop.second;
      auto metaObject = object->metaObject();
      auto metaProperty = metaObject->property(propIndex);
      auto notifySignal = metaProperty.notifySignalIndex();
      QMetaObject::disconnect(object, notifySignal, this,
                              this->metaObject()->indexOfSlot("objectPropertyChanged()"));
    }
  }
  _mappings.clear();
}

MapGroup EditorMapper::pushField(int fieldNumber, int index) {
  auto desc = _desc.empty() ? _rootDesc : _desc.top();
  auto field = desc->FindFieldByNumber(fieldNumber);
  auto group = _model->index(field->index(),0,_index); // << move to field
  _index = _model->index(index,0,group); // << move to first message
  // ask the source for a message pointer
  auto ptr = _model->data(_index,Qt::UserRole+1).value<void*>();
  auto msg = static_cast<Message*>(ptr);
  desc = msg->GetDescriptor();
  _desc.push(desc); // << we're now mapping fields of the message
  return group;
}

void EditorMapper::pushResource() {
  popRoot(); // << just in case
  // ask the source for a message pointer
  auto ptr = _model->data(QModelIndex(),Qt::UserRole+1).value<void*>();
  auto msg = static_cast<TreeNode*>(ptr);
  auto type = msg->type_case();
  R_EXPECT_V(type != TreeNode::TYPE_NOT_SET)
    << "Pushing resource field without a set type!";
  // oneof enum values are equal to field numbers
  pushField(type,0);
}

void EditorMapper::popField() {
  R_EXPECT_V(_index.isValid()) << "Popping beyond root index in mapping!";
  _index = _index.parent(); // << go to the field
  R_EXPECT_V(_index.isValid()) << "Popping from root index in mapping!";
  _index = _index.parent(); // << back to parent message
  if (!_desc.empty()) _desc.pop();
}

void EditorMapper::popRoot() {
  _index = QModelIndex(); // << back to the root
  _desc.clear(); // << back to root descriptor
}

void EditorMapper::load(const MapGroup& group, bool recursive) {
  pushResource(); // << back to the resource automatically

  qDebug() << "byte" << group;
  auto it = _mappings.find(group);
  if (it != _mappings.end()) {
    qDebug() << "hello";
    auto& index = it.key();
    auto& props = it.value();
    for (auto& prop : props) {
      auto object = prop.first;
      auto propIndex = prop.second;
      auto metaObject = object->metaObject();
      auto metaProperty = metaObject->property(propIndex);
      metaProperty.write(object,_model->data(index, Qt::EditRole));
    }
  }
  if (recursive) {

  }
}

void EditorMapper::objectPropertyChanged() {
  auto object = QObject::sender();
  auto signal = QObject::senderSignalIndex();

  //_model->setData(index, metaProperty.read(object), Qt::EditRole);
}
