#ifndef WORDITEMDELEGATE_H
#define WORDITEMDELEGATE_H

#include <QStyledItemDelegate>

class WordItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit WordItemDelegate(QObject *parent = 0);

    QString anchorAt(QString html, const QPoint &point) const;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // WORDITEMDELEGATE_H
