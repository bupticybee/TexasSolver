#include "include/ui/treeitem.h"

TreeItem::TreeItem(weak_ptr<GameTreeNode> data,TreeItem *parentItem) :
    m_parentItem(parentItem)
{
    this->m_treedata = data;
}

bool TreeItem::insertChild(TreeItem *child,int i)
{
    if (i >= m_childItems.count() || i < 0)
        m_childItems.append(child);
    else
        m_childItems.insert(i, child);

    return true;
}

QVariant TreeItem::data() const
{
    return "TEST";
}

bool TreeItem::setParentItem(TreeItem *item)
{
    m_parentItem = item;
    return true;
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}


bool TreeItem::removeChild(int row)
{
    m_childItems.removeAt(row);
    return true;
}
