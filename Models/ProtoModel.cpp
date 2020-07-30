#include "ProtoModel.h"

#include "Components/Logger.h"

#include <QDataStream>
#include <QCoreApplication>

ProtoModel::ProtoModel(QObject *parent, Message *protobuf) : ProtoModel(static_cast<ProtoModel *>(nullptr), protobuf) {
  QObject::setParent(parent);

  const Descriptor *desc = _protobuf->GetDescriptor();
  QSet<QString> uniqueMimes;
  QSet<const Descriptor*> visitedDesc;
  setupMimes(desc, uniqueMimes, visitedDesc);
  _mimes = uniqueMimes.values();
  qDebug() << _mimes;
}

void ProtoModel::setupMimes(const Descriptor* desc, QSet<QString>& uniqueMimes,
                            QSet<const Descriptor*>& visitedDesc) {
  if (!desc) return;
  visitedDesc.insert(desc);
  for (int i = 0; i < desc->field_count(); ++i) {
    const FieldDescriptor *field = desc->field(i);
    if (!field) continue;
    if (field->is_repeated()) {
      std::string typeName = "";
      if (field->type() == FieldDescriptor::TYPE_MESSAGE)
        typeName = field->message_type()->full_name();
      else
        typeName = field->type_name();
      //TODO: Add switch here to use real cross-application cross-process
      //mime type like e.g, application/string for the native field types
      uniqueMimes.insert(QString("RadialGM/") + QString::fromStdString(typeName));
    }
    if (field->type() == FieldDescriptor::TYPE_MESSAGE &&
        !visitedDesc.contains(field->message_type())) // << no infinite recursion
      setupMimes(field->message_type(), uniqueMimes, visitedDesc);
  }
}

ProtoModel::ProtoModel(ProtoModel *parent, Message *protobuf)
    : QAbstractItemModel(parent), _dirty(false), _protobuf(protobuf), _parentModel(parent) {
  connect(this, &ProtoModel::DataChanged, this,
          [this](const QModelIndex &topLeft, const QModelIndex &bottomRight,
                 const QVariant & /*oldValue*/ = QVariant(0), const QVector<int> &roles = QVector<int>()) {
            emit QAbstractItemModel::dataChanged(topLeft, bottomRight, roles);
          });
}

void ProtoModel::ParentDataChanged() {
  ProtoModel *m = GetParentModel<ProtoModel *>();
  while (m != nullptr) {
    emit m->DataChanged(m->index(0, 0), m->index(rowCount() - 1, columnCount() - 1));
    m = m->GetParentModel<ProtoModel *>();
  }
}

void ProtoModel::SetDirty(bool dirty) { _dirty = dirty; }

bool ProtoModel::IsDirty() { return _dirty; }

QModelIndex ProtoModel::parent(const QModelIndex & /*index*/) const { return QModelIndex(); }

Qt::ItemFlags ProtoModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return nullptr;
  auto flags = QAbstractItemModel::flags(index);
  if (canSetData(index))
    flags |= Qt::ItemIsEditable;
  // TODO: check if parent index is repeated
  flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
  return flags;
}

Qt::DropActions ProtoModel::supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

QStringList ProtoModel::mimeTypes() const { return _mimes; }

QMimeData *ProtoModel::mimeData(const QModelIndexList &indexes) const {
  QMimeData *mimeData = new QMimeData();

  QList<QModelIndex> nodes;
  for (const QModelIndex &index : indexes) {
    if (!index.isValid() || nodes.contains(index)) continue;
    nodes << index;
  }

  // rows are moved starting with the lowest so we can create
  // unique names in the order of insertion
  std::sort(nodes.begin(), nodes.end(), std::less<QModelIndex>());

  //TODO: Handle non-message mimes here like string & int
  //ideally pass index to data() and check type of returned
  //variant to get the correct mime type
  QHash<QString,QList<QModelIndex>> indexesByMime;
  for (const QModelIndex &index : nodes) {
    Message *msg = static_cast<Message *>(index.internalPointer());
    std::string typeName = msg->GetTypeName();
    QString mime = "RadialGM/" + QString::fromStdString(typeName);
    indexesByMime[mime].append(index);
  }

  for (auto it = indexesByMime.begin(); it != indexesByMime.end(); ++it) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << QCoreApplication::applicationPid();
    stream << nodes.count();
    for (const QModelIndex &index : it.value()) {
      Message *node = static_cast<Message *>(index.internalPointer());
      stream << reinterpret_cast<qlonglong>(node) << index.row();
    }
    mimeData->setData(it.key(), data);
  }

  return mimeData;
}
