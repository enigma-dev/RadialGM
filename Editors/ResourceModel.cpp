#include "ResourceModel.h"

#include <QDebug>

ResourceModel::ResourceModel(FieldMap fieldMap, QObject *parent) :
	QAbstractListModel(parent), fieldMap(fieldMap) {

}

int ResourceModel::rowCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	return static_cast<int>(fieldMap.size());
}

bool ResourceModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	FieldAddress fieldAddress = fieldMap[static_cast<size_t>(index.row())];
	switch (value.type()) {
		case QVariant::Int: {
			*fieldAddress.int_ptr = value.toInt();
			break;
		}
		case QVariant::String: {
			*fieldAddress.str_ptr = value.toString().toStdString();
			break;
		}
	}
	emit dataChanged(index, index);
	return true;
}

QVariant ResourceModel::data(const QModelIndex &index, int role) const {
	FieldAddress fieldAddress = fieldMap.at(static_cast<size_t>(index.row()));
	return *fieldAddress.int_ptr;
}
