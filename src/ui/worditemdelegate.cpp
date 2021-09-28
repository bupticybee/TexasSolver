#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include "include/ui/worditemdelegate.h"
#include <QRect>
#include <QBrush>

WordItemDelegate::WordItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{}

QString WordItemDelegate::anchorAt(QString html, const QPoint &point) const {
    QTextDocument doc;
    doc.setHtml(html);

    auto textLayout = doc.documentLayout();
    Q_ASSERT(textLayout != 0);
    return textLayout->anchorAt(point);
}

void WordItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto options = option;
    initStyleOption(&options, index);

    painter->save();

    QTextDocument doc;
    doc.setHtml(options.text);

    options.text = "";
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    painter->translate(options.rect.left(), options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    doc.drawContents(painter, clip);

    painter->restore();
}

QSize WordItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem options = option;
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setHtml(options.text);
    doc.setTextWidth(options.rect.width());
    return QSize(doc.idealWidth(), doc.size().height());
}
