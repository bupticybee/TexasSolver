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
    shared_ptr<GameTreeNode> gameTreeNode = this->m_treedata.lock()->getParent();
    shared_ptr<GameTreeNode> currentNode = this->m_treedata.lock();
    if(gameTreeNode == nullptr){
        return "begin";
    }
    if(gameTreeNode->getType() == GameTreeNode::GameTreeNodeType::ACTION){
        shared_ptr<ActionNode> actionNode = dynamic_pointer_cast<ActionNode>(gameTreeNode);
    }
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
