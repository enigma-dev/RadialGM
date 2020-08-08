#ifndef BASEEDTIOR_H
#define BASEEDTIOR_H

#include "Models/EditorModel.h"
#include "Models/EditorMapper.h"

#include <QObject>
#include <QWidget>

class BaseEditor : public QWidget {
  Q_OBJECT

 public:
  explicit BaseEditor(EditorModel *model, QWidget *parent);

 signals:
  // tells the outside world we want edited or opened by editor services
  // for a specific field at an index in our editor model
  // if the index is invalid/the root of editor model then it means the
  // entire resource wants to be edited externally
  void OpenExternally(const QModelIndex &index=QModelIndex());
  void EditExternally(const QModelIndex &index=QModelIndex());

 public slots:
  void OnSave();

 protected:
  virtual void closeEvent(QCloseEvent *event) override;

  EditorMapper *_mapper;
  EditorModel *_model;
};

#endif  // BASEEDTIOR_H
