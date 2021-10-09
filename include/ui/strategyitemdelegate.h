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

private:
    QSolverJob * qSolverJob;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // STRATEGYITEMDELEGATE_H
