#include "EventArgumentsDialog.h"
#include "Components/QMenuView.h"
#include "MainWindow.h"
#include "Models/TreeSortFilterProxyModel.h"
#include "Models/TreeModel.h"
#include "treenode.pb.h"
#include <QDebug>
#include <QAbstractItemModel>

#include <QComboBox>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMetaProperty>
#include <QPushButton>
#include <QSpinBox>
#include <QToolButton>

// Helper function to find the first object in the filtered tree
static QModelIndex findFirstObject(TreeSortFilterProxyModel* proxyModel, const QModelIndex& parent = QModelIndex()) {
  for (int row = 0; row < proxyModel->rowCount(parent); ++row) {
    QModelIndex idx = proxyModel->index(row, 0, parent);
    if (!idx.isValid()) continue;
    
    // Check if this is a folder - skip folders, only return actual objects
    QModelIndex sourceIdx = proxyModel->mapToSource(idx);
    if (sourceIdx.isValid()) {
      TreeModel::Node *node = static_cast<TreeModel::Node *>(sourceIdx.internalPointer());
      if (node) {
        std::string msgType = node->GetMessageType();
        
        // Skip folders - we only want actual objects
        if (msgType == "buffers.TreeNode.Folder") {
          // It's a folder - recurse into children to find an object
          QModelIndex found = findFirstObject(proxyModel, idx);
          if (found.isValid()) return found;
          continue; // Skip this folder
        }
        
        // Check if it's a TreeNode with type_case == kObject
        std::string treeNodeFullName(TreeNode::descriptor()->full_name());
        if (msgType == treeNodeFullName) {
          try {
            TreeNode item = node->GetMessage();
            if (item.type_case() == TreeNode::kFolder) {
              // It's a folder - recurse into children
              QModelIndex found = findFirstObject(proxyModel, idx);
              if (found.isValid()) return found;
              continue;
            }
            // It's an object - return it
            if (item.type_case() == TreeNode::kObject) {
              return idx;
            }
          } catch (...) {
            // If GetMessage() fails, skip
          }
        } else if (msgType == "buffers.resources.Object") {
          // It's an Object resource message - return it
          return idx;
        }
      }
    }
    
    // If this index has children, recurse into them
    if (proxyModel->rowCount(idx) > 0) {
      QModelIndex found = findFirstObject(proxyModel, idx);
      if (found.isValid()) return found;
    }
  }
  return QModelIndex();
}

EventArgumentsDialog::EventArgumentsDialog(QWidget *parent, const QStringList &arguments) : QDialog(parent) {
  arguments_.clear();  // Clear any stale arguments
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
      treeProxy->setSourceModel(MainWindow::treeModel);
      treeProxy->SetFilterType(TreeNode::TypeCase::kObject);  // Set filter AFTER source model
      objMenu->setModel(treeProxy);
      objButton->setMenu(objMenu);
      objButton->setPopupMode(QToolButton::MenuButtonPopup);

      QLineEdit *lineEdit = new QLineEdit(this);
      lineEdit->setReadOnly(true);
      // Since treeProxy is already filtered to show only objects, find the first one
      QModelIndex firstObjIdx = findFirstObject(treeProxy);
      QString firstObj;
      if (firstObjIdx.isValid()) {
        firstObj = firstObjIdx.data(Qt::DisplayRole).toString();
        objButton->setIcon(firstObjIdx.data(Qt::DecorationRole).value<QIcon>());
      } else {
        firstObj = tr("(No objects)");
      }
      lineEdit->setText(firstObj);

      connect(objMenu, &QMenuView::triggered, [=](const QModelIndex &index) {
        // Get the source index to check the actual node type
        QModelIndex sourceIndex = treeProxy->mapToSource(index);
        if (!sourceIndex.isValid()) return;
        
        TreeModel::Node *node = static_cast<TreeModel::Node *>(sourceIndex.internalPointer());
        if (!node) return;
        
        std::string msgType = node->GetMessageType();
        std::string treeNodeFullName(TreeNode::descriptor()->full_name());
        QString displayName = treeProxy->data(index, Qt::DisplayRole).toString();
        
        // Check if this is actually a folder or if it contains objects
        // If msgType is "buffers.TreeNode.Folder", check if it has object children
        // If it does, this is a folder and we should block it
        // If the display name matches an object name and it's in a folder node, 
        // the actual object might be a child
        bool isFolder = false;
        
        if (msgType == "buffers.TreeNode.Folder") {
          // This is a folder node - check if it has any object children
          // If it does, it's a folder and should be blocked
          // But wait - if the displayName is the object name, maybe the object is a child?
          // Actually, in QMenuView, clicking on an object should give us the object node, not the folder
          // So if we get a folder node, it means we clicked on the folder itself
          isFolder = true;
        } else if (msgType == treeNodeFullName) {
          try {
            TreeNode item = node->GetMessage();
            if (item.type_case() == TreeNode::kFolder) {
              isFolder = true;
            }
          } catch (...) {
            // If GetMessage() fails, we can't determine if it's a folder
          }
        }
        
        if (isFolder) {
          // It's a folder - but check if maybe this folder node is being used to represent an object
          // by checking if it has a single object child with the same name
          bool hasObjectChild = false;
          if (msgType == "buffers.TreeNode.Folder" && node->children.size() > 0) {
            // Check if any child is an object
            for (const auto& child : node->children) {
              if (!child) continue;
              std::string childMsgType = child->GetMessageType();
              if (childMsgType == "buffers.resources.Object" || 
                  (childMsgType == treeNodeFullName)) {
                try {
                  TreeNode childItem = child->GetMessage();
                  if (childItem.type_case() == TreeNode::kObject) {
                    hasObjectChild = true;
                    // Use the child object instead
                    std::string childName(childItem.name());
                    if (QString::fromStdString(childName) == displayName) {
                      // This folder contains the object with this name - allow selection
                      lineEdit->setText(displayName);
                      objButton->setIcon(treeProxy->data(index, Qt::DecorationRole).value<QIcon>());
                      return;
                    }
                  }
                } catch (...) {
                  // Skip if we can't get the child
                }
              }
            }
          }
          
          // It's a folder and doesn't contain the object we want, or no matching child found
          return; // Block folder selection
        }
        
        // Allow selection - either it's an object TreeNode or a resource message
        // Update the selection
        lineEdit->setText(displayName);
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
  // Only extract arguments if dialog was accepted
  if (r == QDialog::Accepted) {
    arguments_.clear();  // Clear any stale arguments
    for (const QWidget *w : qAsConst(widgets_)) {
      QString argstr = "";
      
      // Handle different widget types directly for reliability
      if (const QLineEdit *lineEdit = qobject_cast<const QLineEdit*>(w)) {
        argstr = lineEdit->text();
        // Skip placeholder text like "(No objects)"
        if (argstr == tr("(No objects)")) {
          argstr = "";
        }
      } else if (const QSpinBox *spinBox = qobject_cast<const QSpinBox*>(w)) {
        argstr = QString::number(spinBox->value());
      } else if (const QComboBox *combo = qobject_cast<const QComboBox*>(w)) {
        argstr = combo->currentText();
      } else {
        // Fallback to userProperty
        QVariant argument = w->metaObject()->userProperty().read(w);
        if (argument.isValid()) {
          argstr = argument.toString();
        }
      }
      
      arguments_.append(argstr);
    }
  }

  QDialog::done(r);
}
