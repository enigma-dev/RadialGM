#ifndef BASEEDTIOR_H
#define BASEEDTIOR_H

#include "Models/ModelMapper.h"

#include <QObject>
#include <QWidget>

class BaseEditor : public QWidget {
  Q_OBJECT

 public:
  explicit BaseEditor(ProtoModel *model, QWidget *parent);

  virtual void closeEvent(QCloseEvent *event);
  void ReplaceBuffer(google::protobuf::Message *buffer);
  void SetModelData(int index, const QVariant &value);
  QVariant GetModelData(int index);

 public slots:
  virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                           const QVector<int> &roles = QVector<int>());

 protected:
  ModelMapper* resMapper;
};

#endif  // BASEEDTIOR_H
