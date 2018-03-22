#ifndef BASEEDTIOR_H
#define BASEEDTIOR_H

#include "Models/ImmediateMapper.h"
#include "Models/ProtoModel.h"

#include <QObject>
#include <QWidget>

class BaseEditor : public QWidget {
  Q_OBJECT

 public:
  explicit BaseEditor(QWidget *parent, ResourceModel *model);
  QVariant GetModelData(int index);

 protected slots:
  virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                           const QVector<int> &roles = QVector<int>());

 protected:
  ResourceModel *model;
  ImmediateDataWidgetMapper *mapper;
};

#endif  // BASEEDTIOR_H
