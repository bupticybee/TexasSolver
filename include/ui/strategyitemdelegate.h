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
#include "include/ui/tablestrategymodel.h"
#include "include/ui/detailwindowsetting.h"
#include "include/library.h"

class StrategyItemDelegate: public WordItemDelegate{
    Q_OBJECT

public:
    explicit StrategyItemDelegate(QSolverJob * qSolverJob,DetailWindowSetting* detailWindowSetting,QObject *parent = 0);

private:
    QSolverJob * qSolverJob = NULL;
    DetailWindowSetting* detailWindowSetting = NULL;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint_strategy(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, bool withEVs = false) const;
    void paint_range(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint_evs(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // STRATEGYITEMDELEGATE_H
