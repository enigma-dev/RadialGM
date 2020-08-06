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

 public slots:
  void OnSave();

 protected:
  virtual void closeEvent(QCloseEvent *event) override;

  EditorMapper *_mapper;
  EditorModel *_model;
};

#endif  // BASEEDTIOR_H
