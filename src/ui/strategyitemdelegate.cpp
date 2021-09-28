#include "include/ui/strategyitemdelegate.h"
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QRect>
#include <QBrush>

StrategyItemDelegate::StrategyItemDelegate(QObject *parent) :
    WordItemDelegate(parent)
{}

void StrategyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto options = option;
    initStyleOption(&options, index);

    painter->save();

    QRect left_rect(option.rect.left(), option.rect.top(),\
                                     option.rect.width() / 2, option.rect.height());
    QBrush left_brush(Qt::red);
    painter->fillRect(left_rect, left_brush);

    QRect right_rect(option.rect.left() + option.rect.width() / 2, option.rect.top(),\
                                     option.rect.width() / 2 , option.rect.height());
    QBrush right_brush(Qt::green);
    painter->fillRect(right_rect, right_brush);

    QTextDocument doc;
    doc.setHtml(options.text);

    options.text = "";
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    painter->translate(options.rect.left(), options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    doc.drawContents(painter, clip);

    painter->restore();
}
