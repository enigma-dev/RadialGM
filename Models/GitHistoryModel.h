#ifndef GITHISTORYMODEL_H
#define GITHISTORYMODEL_H

#include <QAbstractTableModel>
#include <QVector>

#include <git2.h>

class GitHistoryModel : public QAbstractTableModel
{
public:
  GitHistoryModel(QObject *parent);
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

protected:
  QVector<git_commit*> _commits;
};

#endif // GITHISTORYMODEL_H
