#ifndef STRATEGYITEMDELEGATE_H
#define STRATEGYITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "include/ui/worditemdelegate.h"
#include "include/nodes/GameTreeNode.h"
#include "include/nodes/ActionNode.h"
#include "include/nodes/ChanceNode.h"
#include "include/nodes/TerminalNode.h"
#include "include/nodes/ShowdownNode.h"
#include "include/ui/treeitem.h"
#include "include/runtime/qsolverjob.h"
#include "include/Card.h"

class StrategyItemDelegate: public WordItemDelegate{
    Q_OBJECT

public:
    explicit StrategyItemDelegate(QSolverJob * qSolverJob,QObject *parent = 0);
    void setGameTreeNode(TreeItem* treeItem);
    void setTrunCard(Card turn_card);
    void setRiverCard(Card river_card);

private:
    TreeItem * treeItem = NULL;// = static_cast<TreeItem*>(index.internalPointer());
    QSolverJob * qSolverJob;
    Card turn_card;
    Card river_card;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // STRATEGYITEMDELEGATE_H
