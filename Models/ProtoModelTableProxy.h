#ifndef PROTO_MODEL_TABLE_PROXY_H
#define PROTO_MODEL_TABLE_PROXY_H

#include <QAbstractProxyModel>

class ProtoModelTableProxy {
public:
  ProtoModelTableProxy(QObject* parent, QAbstractItemModel* sourceModel, const QModelIndex& root) : QAbstractProxyModel(parent) {
    setModel(sourceModel);
    setRootIndex(root);
  }
  
  int rowCount(const QModelIndex &parent = QModelIndex()) const override {
    return 0; // repeatedmessage.size() || isMessage() return 1;
  }
  
  int columnCount(const QModelIndex &parent = QModelIndex()) const override {
    return 0; // message type field count (messageK::Whatever)
  }
  
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
    //const Descriptor *desc = _protobuf->GetDescriptor();
    //const FieldDescriptor *field = desc->FindFieldByNumber(section);
    //if (field != nullptr) return QString::fromStdString(field->name());
    return ""; // return the name of kfeild 
  }
  
  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override {
    return QModelIndex(); // map sourceIndex.row, sourceIndex.col to repeatedMessage(index).kField || if isMessage map to kfield
  }
  
  QModelIndex mapToSource(const QModelIndex &proxyIndex) const override {
    return QModelIndex(); // map sourceIndex.row, sourceIndex.col to repeatedMessage(index).kField || if isMessage map to kfield
  }   
  
}; // We can use TransposeModel if we need vertical orientation

#endif
