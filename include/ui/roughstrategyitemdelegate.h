#ifndef ROUGHSTRATEGYITEMDELEGATE_H
#define ROUGHSTRATEGYITEMDELEGATE_H

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
#include "include/ui/detailviewermodel.h"
#include "include/ui/roughstrategyviewermodel.h"
#include "include/library.h"

class RoughStrategyItemDelegate: public WordItemDelegate{
    Q_OBJECT

public:
    explicit RoughStrategyItemDelegate(DetailWindowSetting* detailWindowSetting,QObject *parent = 0);

private:
    DetailWindowSetting* detailWindowSetting = NULL;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint_strategy(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


#endif // ROUGHSTRATEGYITEMDELEGATE_H
