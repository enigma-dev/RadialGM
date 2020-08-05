#ifndef DIAGNOSTICMODEL_H
#define DIAGNOSTICMODEL_H

#include "ProtoModel.h"

#include <QIdentityProxyModel>

// This model adds additional columns and formatting to the super model.
// It is intended for the diagnostic inspector to debug the editor globally.
class DiagnosticModel : public QIdentityProxyModel
{
public:
  explicit DiagnosticModel(ProtoModel *source, QObject *parent);

  virtual QModelIndex index(int row, int column,
                            const QModelIndex &parent = QModelIndex()) const override;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual QVariant data(const QModelIndex &index, int role) const override;
  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role = Qt::DisplayRole) const override;

private:
  ProtoModel *protoModel;
};

#endif // DIAGNOSTICMODEL_H
