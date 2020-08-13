#include "EditorMapper.h"

#include "Editors/BaseEditor.h"
#include "Components/Logger.h"

#include "treenode.pb.h"

EditorMapper::EditorMapper(MessageModel *model, BaseEditor *parent) : QObject(parent),
  _rootModel(model) {
  _rootMapper = new ModelMapper(model, parent);
  _rootMapper->toFirst();
}

void EditorMapper::mapField(int fieldNumber, QWidget *widget, const QByteArray& property) {
  auto mapper = _mappers.isEmpty() ? _rootMapper : _mappers.top();
  mapper->addMapping(widget, fieldNumber, property);
}

void EditorMapper::mapName(QWidget *widget, const QByteArray& property) {
  mapField(TreeNode::kNameFieldNumber, widget, property);
}

EditorMapper::MapGroup EditorMapper::pushField(int fieldNumber, int index) {
  auto mapper = _mappers.isEmpty() ? _rootMapper : _mappers.top();
  auto model = static_cast<MessageModel*>(mapper->GetModel());
  auto submapper = new ModelMapper(model->GetSubModel<MessageModel*>(fieldNumber),
                                   static_cast<BaseEditor*>(QObject::parent()));
  submapper->setCurrentIndex(index);

  _mappers.push(submapper);
  _groups.append(submapper);

  return submapper;
}

void EditorMapper::pushResource() {
  popRoot(); // << just in case
  // ask the source for a message pointer
  buffers::TreeNode* n = static_cast<buffers::TreeNode*>(_rootModel->GetBuffer());
  auto type = n->type_case();
  R_EXPECT_V(type != TreeNode::TYPE_NOT_SET)
    << "Pushing resource field without a set type!";
  // oneof enum values are equal to field numbers
  pushField(type,0);
}

void EditorMapper::pushView(int fieldNumber, QAbstractItemView *view) {
  auto group = pushField(fieldNumber, view->currentIndex().row());
  auto model = static_cast<MessageModel*>(group->GetModel());
  view->setModel(model);
  connect(view->selectionModel(), &QItemSelectionModel::currentRowChanged,
          group, [group](const QModelIndex &current, const QModelIndex &){
    group->setCurrentIndex(current.row());
  });
}

void EditorMapper::popField() {
  R_EXPECT_V(!_mappers.empty()) << "Popping field beyond root mapper!";
  _mappers.pop();
}

void EditorMapper::popRoot() {
  _mappers.clear();
}

void EditorMapper::load(MapGroup group) {
  if (group != nullptr) {
    group->revert();
    return;
  }
  _rootMapper->revert();
  foreach(auto group, _groups) {
    group->revert();
  }
}

void EditorMapper::clear() {
  _rootMapper->clearMapping();
  foreach(auto group, _groups) {
    group->clearMapping();
    delete group;
  }
  _mappers.clear();
  _groups.clear();
}
