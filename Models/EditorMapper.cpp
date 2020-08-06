#include "Models/EditorMapper.h"
#include "Models/EditorModel.h"

#include "Editors/BaseEditor.h"

EditorMapper::EditorMapper(EditorModel *model, BaseEditor *parent) : QObject(parent), _model(model) {
  _mapper = new ImmediateDataWidgetMapper(this);
  _mapper->setOrientation(Qt::Vertical);
  _mapper->setModel(model);
}

void EditorMapper::addMapping(QWidget *widget, int section, QByteArray propName) {
  _mapper->addMapping(widget, section, propName);
}

void EditorMapper::clearMapping() { _mapper->clearMapping(); }

void EditorMapper::toFirst() { _mapper->toFirst(); }
