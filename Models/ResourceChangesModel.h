#ifndef RESOURCECHANGESMODEL_H
#define RESOURCECHANGESMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QMap>

enum class ResChange : int {
  Added,
  Modified,
  Renamed,
  Removed,
  Reverted
};

class ResourceChangesModel : public QAbstractListModel
{
  Q_OBJECT
public:
  ResourceChangesModel(QObject* parent = nullptr);
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual QVariant data(const QModelIndex &index, int role) const override;
  void ResourceChanged(const QString &res, ResChange change);
  void ClearChanges();

protected:
  QMap<QString, ResChange> _changes;
};

#endif // RESOURCECHANGESMODEL_H
