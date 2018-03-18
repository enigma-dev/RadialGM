#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include <QAbstractItemModel>

#include <google/protobuf/message.h>

class ResourceModel : public QAbstractItemModel {
	Q_OBJECT

public:
	ResourceModel(google::protobuf::Message *protobuf, QObject *parent = nullptr);
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role) override;
	QVariant data(const QModelIndex &index, int role) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column = 0,
					  const QModelIndex &parent = QModelIndex()) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
	google::protobuf::Message *protobuf;
};


#endif // RESOURCEMODEL_H
