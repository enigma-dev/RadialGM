#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include <QAbstractItemModel>

#include <google/protobuf/message.h>

class ResourceModel : public QAbstractListModel {
	Q_OBJECT

	google::protobuf::Message *protobuf;

public:
	ResourceModel(google::protobuf::Message *protobuf, QObject *parent = nullptr);
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	QVariant data(const QModelIndex &index, int role) const;
};


#endif // RESOURCEMODEL_H
