#ifndef SPRITESUBIMAGEMODEL_H
#define SPRITESUBIMAGEMODEL_H

#include "RepeatedStringModel.h"

class SpriteSubimageModel : public RepeatedStringModel {
  Q_OBJECT

 signals:
  void MismatchedImageSize(QSize expectedSize, QSize actualSize);

 public:
  SpriteSubimageModel(MutableRepeatedFieldRef<std::string> protobuf, const FieldDescriptor* field,
                      ProtoModelPtr parent);
  void SetMaxIconSize(unsigned width, unsigned height);
  void SetMinIconSize(unsigned width, unsigned height);
  QSize GetIconSize();
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QVariant data(int row) const override;
  QMimeData* mimeData(const QModelIndexList& indexes) const override;
  bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                    const QModelIndex& parent) override;

 protected:
  QSize _maxIconSize;
  QSize _minIconSize;
};

using SpriteSubimageModelPtr = SpriteSubimageModel*;

#endif  // REPEATEDSTRINGMODEL_H
