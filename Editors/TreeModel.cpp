#include "TreeModel.h"
#include "IconManager.h"

#include <QDebug>

TreeModel::TreeModel(buffers::TreeNode* root, QObject *parent)
    : QAbstractItemModel(parent), root(root) {}

TreeModel::~TreeModel()
{
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    buffers::TreeNode *item = static_cast<buffers::TreeNode*>(index.internalPointer());

    if (role == Qt::DecorationRole) {
        if (item->has_folder())
            return IconManager::get_icon("group");
        if (item->has_background())
            return IconManager::get_icon("background");
        if (item->has_font())
            return IconManager::get_icon("font");
        if (item->has_object())
            return IconManager::get_icon("object");
        if (item->has_path())
            return IconManager::get_icon("path");
        if (item->has_room())
            return IconManager::get_icon("room");
        if (item->has_script())
            return IconManager::get_icon("script");
        if (item->has_shader())
            return IconManager::get_icon("shader");
        if (item->has_sound())
            return IconManager::get_icon("sound");
        if (item->has_sprite())
            return IconManager::get_icon("sprite");
        if (item->has_timeline())
            return IconManager::get_icon("timeline");

        return IconManager::get_icon("info");
    }

    if (role != Qt::DisplayRole)
        return QVariant();

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
		return tr("Name");

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

    buffers::TreeNode *childItem = parentItem->mutable_child(row);
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

    return createIndex(parentItem->child_size(), 0, parentItem);
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

    return parentItem->child_size();
}
