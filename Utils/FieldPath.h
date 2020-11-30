#ifndef FIELDPATH_H
#define FIELDPATH_H

#include <QDebug>
#include <QString>
#include <QVector>
#include <google/protobuf/descriptor.h>

class FieldPath {
  using Message = google::protobuf::Message;
  using Descriptor = google::protobuf::Descriptor;
  using FieldDescriptor = google::protobuf::FieldDescriptor;

 public:
  QVector<const FieldDescriptor*> fields;
  QString GetFrom(Message *source);

  FieldPath() = default;
  FieldPath(const FieldDescriptor *fd);
  FieldPath(QVector<const FieldDescriptor*> fds);

  template<typename T, size_t ... fields> static FieldPath Of() {
    QVector<const FieldDescriptor*> fds;
    const Descriptor *md = T::Descriptor();
    for (size_t f : {fields...}) {
      if (!md) break;
      const FieldDescriptor *fd = md->FindFieldByNumber(f);
      if (!fd) {
        qDebug() << "Could not locate field " << f << " in message " << md->full_name().c_str() << "!";
        break;
      }
      fds.push_back(fd);
    }
    return FieldPath(std::move(fds));
  }
};

#endif // FIELDPATH_H
