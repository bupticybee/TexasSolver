#include "include/ui/boardselectortabledelegate.h"

BoardSelectorTableDelegate::BoardSelectorTableDelegate(QStringList ranks,BoardSelectorTableModel *boardSelectorTableModel,QObject *parent):WordItemDelegate(parent){
    this->rank_list = ranks;
    this->boardSelectorTableModel = boardSelectorTableModel;
}

void BoardSelectorTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{

    painter->save();
    auto options = option;
    initStyleOption(&options, index);

    QRect rect(option.rect.left(), option.rect.top(),\
             option.rect.width(), option.rect.height());
    QBrush brush(Qt::gray);
    painter->fillRect(rect, brush);

    float board_float = this->boardSelectorTableModel->getBoardAt(index.row(),index.column());

    float fold_prob = 1 - board_float;
    int disable_height = (int)(fold_prob * option.rect.height());
    int remain_height = option.rect.height() - disable_height;

    rect = QRect(option.rect.left(), option.rect.top() + disable_height,\
     option.rect.width(), remain_height);
    brush = QBrush(Qt::yellow);
    painter->fillRect(rect, brush);

    QTextDocument doc;
    doc.setHtml(Card(options.text.toStdString()).toFormattedHtml());

    painter->translate(options.rect.left(), options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    doc.drawContents(painter, clip);
    painter->restore();
}
