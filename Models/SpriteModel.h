#ifndef SPRITEMODEL_H
#define SPRITEMODEL_H

#include "resources/Sprite.pb.h"

#include <QAbstractListModel>
#include <QHash>
#include <QPixmap>
#include <QSize>

enum SpriteRole : int { FileNameRole = Qt::UserRole, PixmapRole = Qt::UserRole + 1 };

class SpriteModel : public QAbstractListModel {
  Q_OBJECT

 signals:
  void MismatchedImageSize(QSize expectedSize, QSize actualSize);

 public:
  SpriteModel(google::protobuf::RepeatedPtrField<std::string>* protobuf, QObject* parent = nullptr);
  void SetMaxIconSize(unsigned width, unsigned height);
  void SetMinIconSize(unsigned width, unsigned height);
  QSize GetIconSize();
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  bool insertRows(int position, int rows, const QModelIndex& parent) override;
  bool removeRows(int position, int rows, const QModelIndex& parent) override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
  Qt::DropActions supportedDropActions() const override;
  QMimeData* mimeData(const QModelIndexList& indexes) const override;
  bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                    const QModelIndex& parent) override;
  QStringList mimeTypes() const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;

 protected:
  void AddImage(const std::string& fName);

 private:
  google::protobuf::RepeatedPtrField<std::string>* protobuf;
  QSize maxIconSize;
  QSize minIconSize;
  QHash<QString, QPair<QPixmap, QPixmap>> subImages;
};

#endif  // SPRITEMODEL_H
