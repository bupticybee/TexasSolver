#include "include/ui/roughstrategyitemdelegate.h"
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QRect>
#include <QBrush>

RoughStrategyItemDelegate::RoughStrategyItemDelegate(DetailWindowSetting* detailWindowSetting,QObject *parent) :
    WordItemDelegate(parent)
{
    this->detailWindowSetting = detailWindowSetting;
}

void RoughStrategyItemDelegate::paint_strategy(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto options = option;
    initStyleOption(&options, index);

    const RoughStrategyViewerModel * roughStrategyViewerModel = qobject_cast<const RoughStrategyViewerModel*>(index.model());

    options.text = "";
    if(roughStrategyViewerModel->tableStrategyModel->treeItem != NULL &&
            roughStrategyViewerModel->tableStrategyModel->treeItem->m_treedata.lock()->getType() == GameTreeNode::GameTreeNode::ACTION){
        shared_ptr<GameTreeNode> node = roughStrategyViewerModel->tableStrategyModel->treeItem->m_treedata.lock();
        if(index.column() >= roughStrategyViewerModel->tableStrategyModel->total_strategy.size()) return;

        pair<GameActions,pair<float,float>> one_strategy = roughStrategyViewerModel->tableStrategyModel->total_strategy[index.column()];

        QBrush brush(Qt::gray);
        int bet_raise_num = 0;
        for(int i = 0;i <= index.column();i ++){
            GameActions one_action = roughStrategyViewerModel->tableStrategyModel->total_strategy[i].first;
            if(one_action.getAction() == GameTreeNode::PokerActions::FOLD){
                brush = QBrush(QColor	(0,191,255));
            }
            else if(one_action.getAction() == GameTreeNode::PokerActions::CHECK
            || one_action.getAction() == GameTreeNode::PokerActions::CALL){
                brush = QBrush(Qt::green);
            }
            else if(one_action.getAction() == GameTreeNode::PokerActions::BET
            || one_action.getAction() == GameTreeNode::PokerActions::RAISE){
                int color_base = max(128 - 32 * bet_raise_num - 1,0);
                brush = QBrush(QColor(255,color_base,color_base));
                bet_raise_num += 1;
            }else{
                brush = QBrush(Qt::blue);
            }
        }
        QRect rect(option.rect.left(), option.rect.top(),\
             option.rect.width(), option.rect.height());
        painter->fillRect(rect, brush);

        options.text = "";
        GameActions one_action = one_strategy.first;
        float one_strategy_float = one_strategy.second.second * 100;
        float one_combo = one_strategy.second.first;
        if(one_action.getAction() ==  GameTreeNode::PokerActions::FOLD){
            options.text +=  QString(" <h3> %1 <\/h3> <h4>%2\%<\/h4> <h4>%3 %4<\/h4>").arg(tr("FOLD"),QString::number(one_strategy_float,'f',1),QString::number(one_combo,'f',1),tr("combos"));
        }
        else if(one_action.getAction() ==  GameTreeNode::PokerActions::CALL){
            options.text +=  QString(" <h3> %1 <\/h3> <h4>%2\%<\/h4> <h4>%3 %4<\/h4>").arg(tr("CALL"),QString::number(one_strategy_float,'f',1),QString::number(one_combo,'f',1),tr("combos"));

        }
        else if(one_action.getAction() ==  GameTreeNode::PokerActions::CHECK){
            options.text +=  QString(" <h3> %1 <\/h3> <h4>%2\%<\/h4> <h4>%3 %4<\/h4>").arg(tr("CHECK"),QString::number(one_strategy_float,'f',1),QString::number(one_combo,'f',1),tr("combos"));
        }
        else if(one_action.getAction() ==  GameTreeNode::PokerActions::BET){
            options.text +=  QString(" <h3> %1 %2 <\/h3> <h4>%3\%<\/h4> <h4>%4 %5<\/h4>").arg(tr("BET"),QString::number(one_action.getAmount(),'f',1),QString::number(one_strategy_float,'f',1),QString::number(one_combo,'f',1),tr("combos"));
        }
        else if(one_action.getAction() ==  GameTreeNode::PokerActions::RAISE){
            options.text +=  QString(" <h3> %1 %2 <\/h3> <h4>%3\%<\/h4> <h4>%4 %5<\/h4>").arg(tr("RAISE"),QString::number(one_action.getAmount(),'f',1),QString::number(one_strategy_float,'f',1),QString::number(one_combo,'f',1),tr("combos"));
        }

    }
    QTextDocument doc;
    doc.setHtml(options.text);

    options.text = "";
    //options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    painter->translate(options.rect.left(), options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    doc.drawContents(painter, clip);
}


void RoughStrategyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    painter->save();

    QRect rect(option.rect.left(), option.rect.top(),\
             option.rect.width(), option.rect.height());
    QBrush brush(Qt::gray);
    painter->fillRect(rect, brush);

    this->paint_strategy(painter,option,index);

    painter->restore();
}
