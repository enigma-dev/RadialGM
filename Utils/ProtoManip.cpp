#include "ProtoManip.h"

AbstractMessage::AbstractMessage(const AbstractMessage &copy) {
  if (!copy) return;
  message_.reset(copy->New());
  message_->CopyFrom(*copy);
}

AbstractMessage::AbstractMessage(const google::protobuf::Message &message) {
  message_.reset(message.New());
  message_->CopyFrom(message);
}

QVariant GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field) {
  if (field->is_repeated()) {
    return {};
  }
  switch (field->cpp_type()) {
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
      return message.GetReflection()->GetBool(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
      return message.GetReflection()->GetInt32(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
      return (qlonglong) message.GetReflection()->GetInt64(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
      return message.GetReflection()->GetUInt32(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
      return (qulonglong) message.GetReflection()->GetUInt64(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
      return message.GetReflection()->GetFloat(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
      return (qulonglong) message.GetReflection()->GetDouble(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
      return QString::fromStdString(message.GetReflection()->GetString(message, field));
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
      return QVariant::fromValue(AbstractMessage(message.GetReflection()->GetMessage(message, field)));
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
      return QString::fromStdString(message.GetReflection()->GetEnum(message, field)->name());
  }
  return {};
}

template <typename T>
T GetNumeric(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field, T def) {
  if (field->is_repeated()) return def;
  switch (field->cpp_type()) {
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
      return (T) message.GetReflection()->GetBool(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
      return (T) message.GetReflection()->GetInt32(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
      return (T) message.GetReflection()->GetInt64(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
      return (T) message.GetReflection()->GetUInt32(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
      return (T) message.GetReflection()->GetUInt64(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
      return (T) message.GetReflection()->GetFloat(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
      return (T) message.GetReflection()->GetDouble(message, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
      return QString::fromStdString(message.GetReflection()->GetString(message, field));
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
      return QString::fromStdString(message.GetReflection()->GetString(message, field));
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
      return QString::fromStdString(message.GetReflection()->GetEnum(message, field)->name());
  }
  return {};
}

int GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field, int def);
long GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field, long def);
bool GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field, bool def);
float GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field, float def);
double GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field, double def);
const std::string &GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field,
                            const std::string &def);

// Attempts tenaciously to assign the given variant value to the specified field of the given message.
// Performs conversions to try to make this work in degenerate cases (such as assigning string to float or bool).
// If these measures fail, no change is made and the method returns false. Returns true on success.
bool SetField(google::protobuf::Message *message, const google::protobuf::FieldDescriptor *field, const QVariant &val);

QVariant GetField(google::protobuf::MutableRepeatedFieldRef<google::protobuf::int32> repeated_field, int row) {
  return (qint32) repeated_field.Get(row);
}

QVariant GetField(google::protobuf::MutableRepeatedFieldRef<google::protobuf::uint32> repeated_field, int row) {
  return (quint32) repeated_field.Get(row);
}

QVariant GetField(google::protobuf::MutableRepeatedFieldRef<google::protobuf::int64> repeated_field, int row) {
  return (qint64) repeated_field.Get(row);
}

QVariant GetField(google::protobuf::MutableRepeatedFieldRef<google::protobuf::uint64> repeated_field, int row) {
  return (quint64) repeated_field.Get(row);
}

QVariant GetField(google::protobuf::MutableRepeatedFieldRef<std::string> repeated_field, int row) {
  return QString::fromStdString(repeated_field.Get(row));
}

QVariant GetField(google::protobuf::MutableRepeatedFieldRef<google::protobuf::Message> repeated_field, int row) {
  // RepeatedPtrField wants to return a reference, but the field is const and may not have an object allocated already.
  std::unique_ptr<google::protobuf::Message> scratch_space(repeated_field.NewMessage());
  return QVariant::fromValue(AbstractMessage(repeated_field.Get(row, scratch_space.get())));
}

bool SetField(google::protobuf::MutableRepeatedFieldRef<std::string> repeated_field, int row, const QVariant &value) {
  if (!value.canConvert<QString>()) return false;
  repeated_field.Set(row, value.toString().toStdString());
  return true;
}

bool SetField(google::protobuf::MutableRepeatedFieldRef<google::protobuf::Message> repeated_field, int row,
              const QVariant &val) {
  if (auto msg = qvariant_cast<AbstractMessage>(val)) {
    repeated_field.Set(row, *msg);
    return true;
  }
  return false;
}
