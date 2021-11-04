#ifndef TREEITEM_H
#define TREEITEM_H
#include <QList>
#include <QVector>
#include <QVariant>
#include "include/nodes/GameTreeNode.h"
#include "include/nodes/ActionNode.h"
#include "include/nodes/ChanceNode.h"
#include "include/nodes/TerminalNode.h"
#include "include/nodes/ShowdownNode.h"

class TreeItem
{

public:
    weak_ptr<GameTreeNode> m_treedata;

    explicit TreeItem(const weak_ptr<GameTreeNode> tree_node_data,TreeItem *parentItem = 0);
    ~TreeItem() { qDeleteAll(m_childItems); }

    bool insertChild(TreeItem *child,int i=-1);

    TreeItem *child(int row) { return m_childItems.at(row); }
    int childCount() const { return m_childItems.count(); }
    int columnCount() const { return 1; }

    QVariant data() const;
    bool setParentItem(TreeItem *item);
    int row() const;
    TreeItem *parentItem() { return m_parentItem; }
    bool removeChild(int row);

private:
    QList<TreeItem*> m_childItems;
    TreeItem *m_parentItem;
    QString get_round_str(GameTreeNode::GameRound round) const;
    QString get_game_action_str(GameTreeNode::PokerActions action,float amount) const;
};

#endif // TREEITEM_H
