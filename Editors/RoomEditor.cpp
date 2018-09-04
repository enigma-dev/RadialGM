#include "RoomEditor.h"
#include "ui_RoomEditor.h"

#include <QDialog>
#include <QFontDialog>
#include <QGraphicsScene>
#include <QLabel>

RoomEditor::RoomEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::RoomEditor) {
  ui->setupUi(this);

  QGraphicsScene* scene = new QGraphicsScene(this);
  ui->graphicsView->setScene(scene);
}

RoomEditor::~RoomEditor() { delete ui; }
