#ifndef SYSTEMSMODEL_H
#define SYSTEMSMODEL_H

#include "compiler.pb.h"
#include "Settings.pb.h"

#include <QAbstractListModel>

using namespace buffers::resources;

class SystemsModel : public QAbstractListModel
{
public:
    SystemsModel(buffers::SystemType system, QObject* parent = nullptr);
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

private:
    buffers::SystemType system_;
};

#endif // SYSTEMSMODEL_H
