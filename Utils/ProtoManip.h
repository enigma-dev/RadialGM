#ifndef PROTOMANIP_H
#define PROTOMANIP_H

#include <iostream>

#include <QDebug>
#include <QVariant>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>

// =====================================================================================================================
// == Message QVariant storage =========================================================================================
// =====================================================================================================================

class AbstractMessage {
 public:
  AbstractMessage() = default;
  AbstractMessage(const AbstractMessage &copy);
  AbstractMessage(AbstractMessage &&move) = default;

  AbstractMessage(const google::protobuf::Message &message);

  const google::protobuf::Message &operator*() const { return *message_; }
  const google::protobuf::Message *operator->() const { return message_.get(); }
  explicit operator bool() const { return (bool) message_; }

 private:
  std::unique_ptr<google::protobuf::Message> message_;
};

Q_DECLARE_METATYPE(AbstractMessage);
inline std::ostream &operator<<(std::ostream &stream, const AbstractMessage &message) {
  if (message) return stream << message->DebugString();
  return stream << "<null message>";
}
inline QDebug &operator<<(QDebug &stream, const AbstractMessage &message) {
  if (message) return stream << message->DebugString().c_str();
  return stream << "<null message>";
}

// =====================================================================================================================
// == Field access =====================================================================================================
// =====================================================================================================================

// Packages the value of the requested field into a variant.
// Will succeed unless the given field is not present on the given message.
// If the field is null, mismatched for the message, or not present in the message, the empty variant is returned.
QVariant GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field);

namespace protomanip_internal {

int GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field, int);
long GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field, long);
bool GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field, bool);
float GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field, float);
double GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field, double);
const std::string &GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field,
                            const std::string &);

}  // namespace protomanip_internal

template <typename T>
T GetField(const google::protobuf::Message &message, const google::protobuf::FieldDescriptor *field) {
  return protomanip_internal::GetField(message, field, T());
}

// =====================================================================================================================
// == Field assignment =================================================================================================
// =====================================================================================================================

// Attempts tenaciously to assign the given variant value to the specified field of the given message.
// Performs conversions to try to make this work in degenerate cases (such as assigning string to float or bool).
// If these measures fail, no change is made and the method returns false. Returns true on success.
bool SetField(google::protobuf::Message *message, const google::protobuf::FieldDescriptor *field, const QVariant &val);


// =====================================================================================================================
// == Repeated field access ============================================================================================
// =====================================================================================================================

template <typename T>  // Handles bool, double, and float.  QVariant is too picky about the rest.
QVariant GetField(google::protobuf::MutableRepeatedFieldRef<T> repeated_field, int row) {
  return repeated_field.Get(row);
}

QVariant GetField(google::protobuf::MutableRepeatedFieldRef<google::protobuf::int32> repeated_field, int row);
QVariant GetField(google::protobuf::MutableRepeatedFieldRef<google::protobuf::uint32> repeated_field, int row);
QVariant GetField(google::protobuf::MutableRepeatedFieldRef<google::protobuf::int64> repeated_field, int row);
QVariant GetField(google::protobuf::MutableRepeatedFieldRef<google::protobuf::uint64> repeated_field, int row);
QVariant GetField(google::protobuf::MutableRepeatedFieldRef<std::string> repeated_field, int row);

QVariant GetField(google::protobuf::MutableRepeatedFieldRef<google::protobuf::Message> repeated_field, int row);

template <typename number, std::enable_if_t<!std::is_same_v<decltype(QString::number(number{})), void>, bool> = 0>
QString ToQString(number value) { return QString::number(value); }
inline QString ToQString(const std::string &str) { return QString::fromStdString(str); }

// =====================================================================================================================
// == Repeated field assignment ========================================================================================
// =====================================================================================================================

template <typename T>
bool SetField(google::protobuf::MutableRepeatedFieldRef<T> repeated_field, int row, QVariant value) {
  if (!value.convert(QMetaType::fromType<T>().id())) return false;
  repeated_field.Set(row, value.value<T>());
  return true;
}

bool SetField(google::protobuf::MutableRepeatedFieldRef<std::string> repeated_field, int row, const QVariant &value);

bool SetField(google::protobuf::MutableRepeatedFieldRef<google::protobuf::Message> repeated_field, int row,
              const QVariant &val);

// =====================================================================================================================
// == MIME Types =======================================================================================================
// =====================================================================================================================

/// Returns a standardized name for the mime type accepted by the given field, whether string, integer, or message.
/// For message fields, only accepts exactly that message type.
QString GetMimeType(const google::protobuf::FieldDescriptor *desc);

/// Returns ALL mime types acceptable to a given message or fields within that message. Useful for models that display
/// all fields within a given message (namely, TreeModel).
QStringList GetMimeTypes(const google::protobuf::Descriptor *desc);

#endif // PROTOMANIP_H
