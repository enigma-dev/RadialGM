#include "Models/EditorMapper.h"
#include "Models/EditorModel.h"

#include "Editors/BaseEditor.h"

EditorMapper::EditorMapper(EditorModel *model, BaseEditor *parent) : QObject(parent),
  _model(model) {}
