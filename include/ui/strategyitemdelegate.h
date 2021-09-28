#ifndef STRATEGYITEMDELEGATE_H
#define STRATEGYITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "include/ui/worditemdelegate.h"

class StrategyItemDelegate: public WordItemDelegate{
    Q_OBJECT

public:
    explicit StrategyItemDelegate(QObject *parent = 0);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // STRATEGYITEMDELEGATE_H
