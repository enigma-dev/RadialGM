#include "RoomEditor.h"
#include "ui_RoomEditor.h"

#include <QDialog>
#include <QFontDialog>
#include <QGraphicsScene>
#include <QLabel>

RoomEditor::RoomEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::RoomEditor) {
  ui->setupUi(this);

  QGraphicsScene* scene = new QGraphicsScene(this);

  auto dialog = new QDialog();
  dialog->setWindowTitle("Legend");
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  QVBoxLayout* verticaLayout = new QVBoxLayout();
  QLabel* imageLabel = new QLabel();
  imageLabel->setPixmap(QPixmap("C:/Users/Owner/Desktop/minimap.png").scaled(128, 128));
  verticaLayout->addWidget(imageLabel);
  dialog->setLayout(verticaLayout);

  scene->addWidget(dialog, Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowTitleHint |
                               Qt::WindowSystemMenuHint);

  ui->graphicsView->setScene(scene);
}

RoomEditor::~RoomEditor() { delete ui; }
