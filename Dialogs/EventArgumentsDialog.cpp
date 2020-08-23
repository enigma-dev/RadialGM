#include "EventArgumentsDialog.h"
#include "MainWindow.h"
#include "Components/QMenuView.h"
#include "Models/TreeSortFilterProxyModel.h"

#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QToolButton>
#include <QLineEdit>

EventArgumentsDialog::EventArgumentsDialog(QWidget* parent, const QStringList& arguments)
  : QDialog(parent) {
  QGridLayout* layout = new QGridLayout(this);

  int row = 0;
  for (const auto& arg : arguments) {
    QLabel* name = new QLabel(this);
    name->setText(arg);

    QWidget* value;

    if (arg == "integer") {
      QSpinBox* integer = new QSpinBox(this);
      integer->setMinimum(0);
      value = integer;
      layout->addWidget(value, row, 1);
    } else if (arg == "object") {
      QHBoxLayout* objLayout = new QHBoxLayout(this);
      QToolButton* objButton = new QToolButton(this);

      QMenuView* objMenu = new QMenuView(this);
      TreeSortFilterProxyModel* treeProxy = new TreeSortFilterProxyModel(this);
      treeProxy->SetFilterType(TreeNode::TypeCase::kObject);
      treeProxy->setSourceModel(MainWindow::treeModel.get());
      objMenu->setModel(treeProxy);
      objButton->setMenu(objMenu);
      objButton->setPopupMode(QToolButton::MenuButtonPopup);
      objButton->setIcon(ArtManager::GetIcon(":/resources/object.png"));

      QLineEdit* lineEdit = new QLineEdit(this);
      lineEdit->setDisabled(true);

      connect(objMenu, &QMenuView::triggered, [=](const QModelIndex& index) {
        lineEdit->setText(treeProxy->data(index, Qt::DisplayRole).toString());
        objButton->setIcon(treeProxy->data(index, Qt::DecorationRole).value<QIcon>());
      });

      objLayout->addWidget(lineEdit);
      objLayout->addWidget(objButton);

      layout->addItem(objLayout, row, 1);

      value = lineEdit;
    } else {
      QComboBox* combo = new QComboBox(this);
      //combo->setModel()
      value = combo;
      layout->addWidget(value, row, 1);
    }

    widgets_.append(value);

    layout->addWidget(name, row, 0);
    row++;
  }

  QDialogButtonBox* btn = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  layout->addWidget(btn, row, 1);
  connect(btn, SIGNAL(accepted()), this, SLOT(accept()));
  connect(btn, SIGNAL(rejected()), this, SLOT(reject()));

  setLayout(layout);
  setWindowTitle(tr("Event arguments"));
}

const QStringList& EventArgumentsDialog::GetArguments() const {
  return arguments_;
}

void EventArgumentsDialog::done(int r) {
 for (const QWidget* w : widgets_) {
QVariant argument = w->metaObject()->userProperty()->read(w);
QString argstr = "";
if (argument.isValid() && argument.type() == QMetaType::QString)
  argstr = argument.toString();
arguments_.append(argstr);
 }

 qDebug() << "argumemts sixe: " << arguments_.size();

 QDialog::done(r);
}

QSize EventArgumentsDialog::sizeHint() const {
  return QSize(300,300);
}
