#ifndef REPEATEDSTRINGMODEL_H
#define REPEATEDSTRINGMODEL_H

#include "RepeatedModel.h"

#include <string>

class RepeatedStringModel : public RepeatedPrimitiveModel<std::string> {
 public:
  RepeatedStringModel(ProtoModel *parent, Message *message, const FieldDescriptor *field)
      : RepeatedPrimitiveModel<std::string>(parent, message, field) {}

  QString DebugName() const override {
    return QString::fromStdString("RepeatedStringModel<" + _field->full_name() + ">");
  }
  RepeatedStringModel *TryCastAsRepeatedStringModel() override { return this; }
};

#endif  // REPEATEDSTRINGMODEL_H
