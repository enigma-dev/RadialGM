#ifndef FIELDPATH_H
#define FIELDPATH_H

#include <QDebug>
#include <QString>
#include <google/protobuf/descriptor.h>
#include <vector>

class FieldPath {
  using Message = google::protobuf::Message;
  using Descriptor = google::protobuf::Descriptor;
  using FieldDescriptor = google::protobuf::FieldDescriptor;

 public:
  struct FieldComponent {
    const google::protobuf::FieldDescriptor *field;
    int repeated_field_index = -1;  ///< Repeated field index, only if field->is_repeated(). Otherwise, -1.

    // Convenience method to access field data.
    const google::protobuf::FieldDescriptor *operator->() const { return field; }

    FieldComponent(const google::protobuf::FieldDescriptor *field, int index = -1):
        field(field), repeated_field_index(index) {}
  };
  std::vector<FieldComponent> fields;
  int repeated_field_index = -1;  ///< Repeated field index, only when accessing a repeated model. Otherwise, -1.
  QString GetFrom(Message *source);

  FieldPath() = default;
  explicit FieldPath(const FieldDescriptor *fd);
  explicit FieldPath(std::vector<FieldComponent> fields): FieldPath(-1, fields) {}
  explicit FieldPath(int start_position, std::vector<FieldComponent> fields);

  FieldComponent front() const { return fields.front(); }
  size_t size() const { return fields.size(); }

  struct FCTag {
    int field_number;
    int repeated_field_index = -1;

    constexpr FCTag(int f): field_number(f) {}
    constexpr FCTag(int f, int idx): field_number(f), repeated_field_index(idx) {}
  };

  /// May appear as the first element in a FieldPath to specify that the path itself starts at the Nth element of a
  /// repeated model. All other repeated field index operations use RepeatedOffset(), which specifies a field number.
  static constexpr FCTag StartingAt(int index) { return {0, index}; }
  /// Used to access a particular index of a repeated field.
  static constexpr FCTag RepeatedOffset(int field_num, int index) { return {field_num, index}; }

  template<typename T, typename ... Fields> static FieldPath Of(Fields... field_components) {
    std::vector<FieldComponent> fields;
    const Descriptor *md = T::GetDescriptor();
    bool first_element = true;
    int start_index = -1;
    for (FCTag fct : std::initializer_list<FCTag>{field_components...}) {
      if (!md) break;
      if (first_element) {
        first_element = false;
        if (!fct.field_number) {
          start_index = fct.repeated_field_index;
          continue;
        }
      }
      const FieldDescriptor *fd = md->FindFieldByNumber(fct.field_number);
      if (!fd) {
        qDebug() << "Could not locate field " << fct.field_number << " in message " << md->full_name().c_str() << "!";
        break;
      }
      if (fct.repeated_field_index != -1 && !fd->is_repeated()) {
        qDebug() << "Attempting to treat field " << fd->name().c_str() << " in message " << md->full_name().c_str()
                 << " as a repeated field!";
        break;
      }
      fields.emplace_back(fd, fct.repeated_field_index);
      md = fd->message_type();
    }
    return FieldPath(start_index, std::move(fields));
  }

  FieldPath SkipField() const {
    if (fields.empty()) return FieldPath();
    return FieldPath(fields.front().repeated_field_index, {fields.begin() + 1, fields.end()});
  }

  FieldPath SkipIndex() const {
    return FieldPath(-1, {fields.begin(), fields.end()});
  }

  std::string str() const {
    std::string field = "";
    for (const auto &fcomp : fields) {
      if (!field.empty()) field += ".";
      field += fcomp->name();
    }
    return field;
  }

  // Returns the concatenation of the two field paths, if the first field of the right-hand path is a
  // field of the last field of the left-hand path. Otherwise, the left-hand path is returned as-is.
  FieldPath operator+(const FieldPath &field_path) const;

  explicit operator bool() const { return fields.size(); }
};

#endif // FIELDPATH_H
