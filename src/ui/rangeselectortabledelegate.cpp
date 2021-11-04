#include "include/ui/rangeselectortabledelegate.h"

RangeSelectorTableDelegate::RangeSelectorTableDelegate(QStringList ranks,RangeSelectorTableModel *rangeSelectorTableModel,QObject *parent):WordItemDelegate(parent){
    this->rank_list = ranks;
    this->rangeSelectorTableModel = rangeSelectorTableModel;
}

void RangeSelectorTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{

    painter->save();
    auto options = option;
    initStyleOption(&options, index);

    QRect rect(option.rect.left(), option.rect.top(),\
             option.rect.width(), option.rect.height());
    QBrush brush(Qt::gray);
    if(index.column() == index.row())brush = QBrush(Qt::darkGray);
    painter->fillRect(rect, brush);

    float range_float = this->rangeSelectorTableModel->getRangeAt(index.row(),index.column());

    float fold_prob = 1 - range_float;
    int disable_height = (int)(fold_prob * option.rect.height());
    int remain_height = option.rect.height() - disable_height;

    rect = QRect(option.rect.left(), option.rect.top() + disable_height,\
     option.rect.width(), remain_height);
    brush = QBrush(Qt::yellow);
    painter->fillRect(rect, brush);

    QTextDocument doc;
    doc.setHtml(options.text);

    painter->translate(options.rect.left(), options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    if(!this->rangeSelectorTableModel->in_thumbnail_mode()){
        doc.drawContents(painter, clip);
    }
    painter->restore();
}
