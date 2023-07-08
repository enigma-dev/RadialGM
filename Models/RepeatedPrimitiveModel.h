#ifndef REPEATEDPRIMITIVEMODEL_H
#define REPEATEDPRIMITIVEMODEL_H

#include "RepeatedModel.h"
#include "PrimitiveModel.h"

template <typename T>
class RepeatedPrimitiveModel : public BasicRepeatedModel<T> {
 public:
  RepeatedPrimitiveModel(ProtoModel *parent, Message *message, const FieldDescriptor *field) : BasicRepeatedModel<T>(
      parent, message, field, message->GetReflection()->GetMutableRepeatedFieldRef<T>(message, field)) {
    RebuildSubModels();
  }

  T PrimitiveData(int row) const {
    return BasicRepeatedModel<T>::field_ref_.Get(row);
  }

  virtual QString FastGetQString(int row) const override {
    return ToQString(PrimitiveData(row));
  }

  void SwapWithoutSignal(int left, int right) override {
    BasicRepeatedModel<T>::SwapWithoutSignal(left, right);
    ProtoModel::SwapModels(submodels_[left], submodels_[right]);
  }

  // Need to implement this in all RepeatedModels
  void AppendNewWithoutSignal() final {
    BasicRepeatedModel<T>::field_ref_.Add({});
    submodels_.push_back(new PrimitiveModel(this, BasicRepeatedModel<T>::field_ref_.size() - 1));
  }

  void RebuildSubModels() {
    submodels_.clear();
    submodels_.reserve(BasicRepeatedModel<T>::field_ref_.size());
    for (int i = 0; i < BasicRepeatedModel<T>::field_ref_.size(); ++i) {
      submodels_.push_back(new PrimitiveModel(this, i));
    }
  }

  ProtoModel *GetSubModel(int index) const final {
    R_EXPECT(index < submodels_.size(), nullptr) << "Requested submodel index: " << index << "is out of range";
    return submodels_[index];
  }

 private:
  QVector<PrimitiveModel*> submodels_;
};

#define RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(ModelName, model_type)                     \
    class ModelName : public RepeatedPrimitiveModel<model_type> {                       \
     public:                                                                            \
      ModelName(ProtoModel *parent, Message *message, const FieldDescriptor *field)     \
          : RepeatedPrimitiveModel<model_type>(parent, message, field) {}               \
                                                                                        \
      QString DebugName() const override {                                              \
        return QString::fromStdString(#ModelName "<" + field_->full_name() + ">");      \
      }                                                                                 \
      ModelName *TryCastAs ## ModelName() override { return this; }                     \
    }

RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedStringModel, std::string);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedBoolModel,   bool);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedInt32Model,  google::protobuf::int32);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedInt64Model,  google::protobuf::int64);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedUInt32Model, google::protobuf::uint32);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedUInt64Model, google::protobuf::uint64);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedFloatModel,  float);
RGM_DECLARE_REPEATED_PRIMITIVE_MODEL(RepeatedDoubleModel, double);

#undef RGM_DECLARE_REPEATED_PRIMITIVE_MODEL

#endif
