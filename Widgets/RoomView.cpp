#include "RoomView.h"
#include "Components/ArtManager.h"
#include "Components/Logger.h"
#include "MainWindow.h"
#include "Models/ProtoModel.h"

#include <QDebug>
#include <QPainter>

bool InstanceSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const {
  return false;
}

RoomView::RoomView(AssetScrollAreaBackground* parent) : AssetView(parent) {
  setFixedSize(sizeHint());
  this->_sortedInstances = new InstanceSortFilterProxyModel(this);
  this->_sortedTiles = new QSortFilterProxyModel(this);
}



QSize RoomView::sizeHint() const {

}

void RoomView::Paint(QPainter& painter) {

}

void RoomView::paintTiles(QPainter& painter) {

}

void RoomView::paintBackgrounds(QPainter& painter, bool foregrounds) {

}

void RoomView::paintInstances(QPainter& painter) {

}
