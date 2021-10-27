#ifndef BOARDSELECTORTABLEDELEGATE_H
#define BOARDSELECTORTABLEDELEGATE_H

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
#include "include/library.h"
#include "include/ui/boardselectortablemodel.h"
#include <QStringList>
#include <QPainter>

class BoardSelectorTableDelegate: public WordItemDelegate{
    Q_OBJECT
public:
    explicit BoardSelectorTableDelegate(QStringList ranks,BoardSelectorTableModel* boardSelectorTableModel,QObject *parent = 0);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    BoardSelectorTableModel *boardSelectorTableModel;
    QStringList rank_list;
};

#endif // BOARDSELECTORTABLEDELEGATE_H
