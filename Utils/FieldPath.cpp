#include "FieldPath.h"

FieldPath::FieldPath(const FieldDescriptor *fd) {
  if (fd) {
    fields.emplace_back(fd);
  } else {
    qDebug() << "Constructed FieldPath with null FieldDescriptor!";
  }
}
FieldPath::FieldPath(std::vector<FieldComponent> fields_in) {
  for (const FieldComponent &field : fields_in) {
    if (field.field) {
      fields.emplace_back(field);
    } else {
      qDebug() << "Constructed FieldPath with a vector containing null FieldDescriptors!";
    }
  }
}

FieldPath FieldPath::operator+(const FieldPath &field_path) const {
  if (fields.empty()) return field_path;
  if (field_path.fields.empty()) return *this;
  if (!fields.back()->message_type()) return *this;

  const FieldDescriptor *first = field_path.front().field;
  if (fields.back()->message_type()->FindFieldByNumber(first->number()) != first) return *this;

  FieldPath res = *this;
  res.fields.insert(res.fields.end(), field_path.fields.begin(), field_path.fields.end());
  return res;
}
