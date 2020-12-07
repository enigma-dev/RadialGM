#ifndef SPRITESUBIMAGEMODEL_H
#define SPRITESUBIMAGEMODEL_H

#include "RepeatedStringModel.h"

#include <QSize>

class RepeatedImageModel : public RepeatedStringModel {
  Q_OBJECT

 public:
  RepeatedImageModel(ProtoModel* parent, Message* message, const FieldDescriptor* field)
      : RepeatedStringModel(parent, message, field), _maxIconSize(128, 128), _minIconSize(16, 16) {}

  void SetMaxIconSize(unsigned width, unsigned height);
  void SetMinIconSize(unsigned width, unsigned height);
  QSize GetIconSize();

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QMimeData* mimeData(const QModelIndexList& indexes) const override;
  bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                    const QModelIndex& parent) override;

  virtual RepeatedImageModel *AsRepeatedImageModel() { return this; }

 signals:
  void MismatchedImageSize(QSize expectedSize, QSize actualSize);

 protected:
  QSize _maxIconSize;
  QSize _minIconSize;
};

#endif  // REPEATEDSTRINGMODEL_H
