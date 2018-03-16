#include "TreeModel.h"

TreeModel::TreeModel(buffers::TreeNode* root, QObject *parent)
    : QAbstractItemModel(parent), root(root) {}

TreeModel::~TreeModel()
{
    delete root;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    buffers::TreeNode *item = static_cast<buffers::TreeNode*>(index.internalPointer());
    return QString::fromStdString(item->name());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return QString::fromStdString(root->name());

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    buffers::TreeNode *parentItem;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<buffers::TreeNode*>(parent.internalPointer());

    buffers::TreeNode *childItem = parentItem->mutable_node(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();

}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    buffers::TreeNode* childItem = static_cast<buffers::TreeNode*>(index.internalPointer());
    buffers::TreeNode* parentItem = childItem->mutable_parent();

    if (parentItem == root)
        return QModelIndex();

    return createIndex(parentItem->node_size(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    buffers::TreeNode* parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<buffers::TreeNode*>(parent.internalPointer());

    return parentItem->node_size();
}

/*void TreeModel::setupModelData(const QStringList &lines, TreeItem *parent)
{
    QList<TreeItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].at(position) != ' ')
                break;
            position++;
        }

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            parents.last()->appendChild(new TreeItem(columnData, parents.last()));
        }

        ++number;
    }
}*/
