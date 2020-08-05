#include "DiagnosticModel.h"

#include <QFont>

DiagnosticModel::DiagnosticModel(ProtoModel *source, QObject *parent) :
  QIdentityProxyModel(parent), protoModel(source)
{
  this->setSourceModel(source);
}

QModelIndex DiagnosticModel::index(int row, int column, const QModelIndex &parent) const {
  auto index = QIdentityProxyModel::index(row, 0, parent);

  // all other columns are virtual columns we've added
  // QIdentityProxyModel just uses an iterator as internal id
  // for its mapped indexes so we just give the other columns
  // the same internal id so they can be mapped into the source
  if (column != 0) {
    return this->createIndex(row, column, index.internalId());
  }

  return index;
}

int DiagnosticModel::columnCount(const QModelIndex &/*parent*/) const {
  return 2;
}

QVariant DiagnosticModel::data(const QModelIndex &index, int role) const {
  switch (index.column()) {
    case 0: return QIdentityProxyModel::data(index, role);
    case 1: {
      if (role == Qt::FontRole) {
        // keep address column neatly aligned
        QFont font = QIdentityProxyModel::data(index, role).value<QFont>();
        font.setStyleHint(QFont::Monospace, QFont::PreferMatch);
        return font;
      }
      if (role != Qt::DisplayRole) break;
      auto sourceIndex = mapToSource(index);
      return QString::number((quintptr)sourceIndex.internalId(), 16).toUpper();
    }
  }
  return QVariant();
}

QVariant DiagnosticModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  switch (section) {
  case 0: return tr("Name");
  case 1: return tr("Address");
  }

  return QVariant();
}
