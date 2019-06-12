#include "ObjectEditor.h"

#include "ui_ObjectEditor.h"

/*#include <QMessageBox>
QMessageBox::information(this,tr(""),tr("Hiya!"));*/

ObjectEditor::ObjectEditor(ProtoModel* model, QWidget* parent) : BaseEditor(model, parent), ui(new Ui::ObjectEditor) {
  ui->setupUi(this);
}

ObjectEditor::~ObjectEditor() { delete ui; }

void ObjectEditor::on_saveButton_released()
{
    QWidget *parent = parentWidget();
    parent->close(); //pass just save in future
}
