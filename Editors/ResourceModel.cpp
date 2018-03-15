#include "ResourceModel.h"

#include <QDebug>

ResourceModel::ResourceModel(google::protobuf::Message *protobuf, QObject *parent) :
	QAbstractListModel(parent), protobuf(protobuf) {}

int ResourceModel::rowCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	const google::protobuf::Descriptor *desc = protobuf->GetDescriptor();
	return desc->field_count();
}

bool ResourceModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	const google::protobuf::Descriptor *desc = protobuf->GetDescriptor();
	const google::protobuf::Reflection *refl = protobuf->GetReflection();
	const google::protobuf::FieldDescriptor *field = desc->field(index.row());

	switch (field->cpp_type()) {
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE: {

			break;
		}
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
			refl->SetInt32(protobuf, field, value.toInt());
		break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
			refl->SetInt64(protobuf, field, value.toLongLong());
		break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
			refl->SetUInt32(protobuf, field, value.toUInt());
		break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
			refl->SetUInt64(protobuf, field, value.toULongLong());
		break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
			refl->SetDouble(protobuf, field, value.toDouble());
		break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
			refl->SetFloat(protobuf, field, value.toFloat());
		break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
			refl->SetBool(protobuf, field, value.toBool());
		break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
			refl->SetEnum(protobuf, field, field->enum_type()->FindValueByNumber(value.toInt()));
		break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
			refl->SetString(protobuf, field, value.toString().toStdString());
		break;
	}
	emit dataChanged(index, index);
	return true;
}

QVariant ResourceModel::data(const QModelIndex &index, int role) const {
	const google::protobuf::Descriptor *desc = protobuf->GetDescriptor();
	const google::protobuf::Reflection *refl = protobuf->GetReflection();
	const google::protobuf::FieldDescriptor *field = desc->field(index.row());

	switch (field->cpp_type()) {
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
			return refl->GetInt32(*protobuf, field);
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
			return refl->GetInt32(*protobuf, field);
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
			return static_cast<long long>(refl->GetInt64(*protobuf, field));
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
			return refl->GetUInt32(*protobuf, field);
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
			return static_cast<unsigned long long>(refl->GetUInt64(*protobuf, field));
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
			return refl->GetDouble(*protobuf, field);
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
			return refl->GetFloat(*protobuf, field);
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
			return refl->GetBool(*protobuf, field);
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
			return refl->GetInt32(*protobuf, field);
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
			return refl->GetString(*protobuf, field).c_str();
	}
	return 0;
}
