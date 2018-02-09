#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include <QAbstractItemModel>

#include <unordered_map>

union FieldAddress {
	int* int_ptr;
	std::string* str_ptr;
};

typedef std::unordered_map<size_t, FieldAddress> FieldMap;

class ResourceModel : public QAbstractListModel {
	Q_OBJECT

	FieldMap fieldMap;

public:
	ResourceModel(FieldMap fieldMap, QObject *parent = nullptr);
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	QVariant data(const QModelIndex &index, int role) const;
};


#endif // RESOURCEMODEL_H
