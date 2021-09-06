#include "EventArgumentsDialog.h"
#include "Components/QMenuView.h"
#include "MainWindow.h"
#include "Models/TreeSortFilterProxyModel.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMetaProperty>
#include <QPushButton>
#include <QSpinBox>
#include <QToolButton>

EventArgumentsDialog::EventArgumentsDialog(QWidget *parent, const QStringList &arguments) : QDialog(parent) {
  QGridLayout *layout = new QGridLayout(this);

  int row = 0;
  for (const auto &arg : arguments) {
    QLabel *name = new QLabel(this);
    name->setText(arg);

    QWidget *value;

    if (arg == "integer") {
      QSpinBox *integer = new QSpinBox(this);
      integer->setMinimum(0);
      value = integer;
      layout->addWidget(value, row, 1);
    } else if (arg == "string") {
      QLineEdit *lineEdit = new QLineEdit(this);
      lineEdit->setText(tr("MyCustomEvent"));
      value = lineEdit;
      layout->addWidget(value, row, 1);
    } else if (arg == "object") {
      QHBoxLayout *objLayout = new QHBoxLayout();
      QToolButton *objButton = new QToolButton(this);

      QMenuView *objMenu = new QMenuView(this);
      TreeSortFilterProxyModel *treeProxy = new TreeSortFilterProxyModel(this);
      treeProxy->SetFilterType(TreeNode::TypeCase::kObject);
      treeProxy->setSourceModel(MainWindow::treeModel.get());
      objMenu->setModel(treeProxy);
      objButton->setMenu(objMenu);
      objButton->setPopupMode(QToolButton::MenuButtonPopup);

      QLineEdit *lineEdit = new QLineEdit(this);
      lineEdit->setReadOnly(true);
      auto object_message_name = QString::fromStdString(buffers::resources::Object::descriptor()->full_name());
      auto search = treeProxy->match(treeProxy->index(0, 0), TreeModel::UserRoles::MessageTypeRole, object_message_name,
                                     1, Qt::MatchRecursive);
      QModelIndex firstObjIdx = search.first();

      QString firstObj = firstObjIdx.data(Qt::DisplayRole).toString();
      objButton->setIcon(firstObjIdx.data(Qt::DecorationRole).value<QIcon>());
      lineEdit->setText(firstObj);

      connect(objMenu, &QMenuView::triggered, [=](const QModelIndex &index) {
        lineEdit->setText(treeProxy->data(index, Qt::DisplayRole).toString());
        objButton->setIcon(treeProxy->data(index, Qt::DecorationRole).value<QIcon>());
      });

      objLayout->addWidget(lineEdit);
      objLayout->addWidget(objButton);

      layout->addItem(objLayout, row, 1);

      value = lineEdit;
    } else {
      QComboBox *combo = new QComboBox(this);
      auto argList = MainWindow::GetEventData()->value_names_for_type(name->text().toStdString());
      for (auto& a : argList) {
        combo->addItem(QString::fromStdString(a.first));
      }
      value = combo;
      layout->addWidget(value, row, 1);
    }

    widgets_.append(value);

    layout->addWidget(name, row, 0);
    row++;
  }

  QDialogButtonBox *btn = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  layout->addWidget(btn, row, 1);
  connect(btn, SIGNAL(accepted()), this, SLOT(accept()));
  connect(btn, SIGNAL(rejected()), this, SLOT(reject()));

  setWindowTitle(tr("Event arguments"));
}

const QStringList &EventArgumentsDialog::GetArguments() const { return arguments_; }

void EventArgumentsDialog::done(int r) {
  for (const QWidget *w : qAsConst(widgets_)) {
    QVariant argument = w->metaObject()->userProperty().read(w);
    QString argstr = "";
    if (argument.isValid())
      argstr = argument.toString();
    arguments_.append(argstr);
  }

  QDialog::done(r);
}
