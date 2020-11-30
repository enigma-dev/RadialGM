#include "FieldPath.h"

FieldPath::FieldPath(const FieldDescriptor *fd) {
  if (fd) {
    fields.push_back(fd);
  } else {
    qDebug() << "Constructed FieldPath with null FieldDescriptor!";
  }
}
FieldPath::FieldPath(QVector<const FieldDescriptor*> fds) {
  for (const FieldDescriptor *fd: fds) {
    if (fd) {
      fields.push_back(fd);
    } else {
      qDebug() << "Constructed FieldPath with a vector containing null FieldDescriptors!";
    }
  }
}
