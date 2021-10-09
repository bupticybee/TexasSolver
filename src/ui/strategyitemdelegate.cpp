#include "include/ui/strategyitemdelegate.h"
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QRect>
#include <QBrush>

StrategyItemDelegate::StrategyItemDelegate(QSolverJob * qSolverJob,QObject *parent) :
    WordItemDelegate(parent)
{
    this->qSolverJob = qSolverJob;
}

void StrategyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto options = option;
    initStyleOption(&options, index);

    painter->save();

    QRect rect(option.rect.left(), option.rect.top(),\
             option.rect.width(), option.rect.height());
    QBrush brush(Qt::gray);
    painter->fillRect(rect, brush);


    QTextDocument doc;
    doc.setHtml(options.text);

    options.text = "";
    //options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    painter->translate(options.rect.left(), options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    doc.drawContents(painter, clip);

    painter->restore();
}
