#ifndef ASSETPREVIEWSCROLLAREA_H
#define ASSETPREVIEWSCROLLAREA_H

#include <QScrollArea>

class AssetScrollArea : public QScrollArea {
  Q_OBJECT
 public:
  explicit AssetScrollArea(QWidget* parent) : QScrollArea(parent) {}
};

#endif  // ASSETPREVIEWSCROLLAREA_H
