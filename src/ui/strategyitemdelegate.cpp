#include "include/ui/strategyitemdelegate.h"
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QRect>
#include <QBrush>

StrategyItemDelegate::StrategyItemDelegate(QSolverJob * qSolverJob,DetailWindowSetting* detailWindowSetting,QObject *parent) :
    WordItemDelegate(parent)
{
    this->detailWindowSetting = detailWindowSetting;
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

    const TableStrategyModel * tableStrategyModel = qobject_cast<const TableStrategyModel*>(index.model());
    vector<pair<GameActions,float>> strategy = tableStrategyModel->get_strategy(index.row(),index.column());
    if(!strategy.empty()){
        float fold_prob = 0;
        vector<float> strategy_without_fold;
        float strategy_without_fold_sum = 0;
        for(int i = 0;i < strategy.size();i ++){
            GameActions one_action = strategy[i].first;
            if(one_action.getAction() == GameTreeNode::PokerActions::FOLD){
                fold_prob = strategy[i].second;
            }else{
                strategy_without_fold.push_back(strategy[i].second);
                strategy_without_fold_sum += strategy[i].second;
            }
        }

        for(int i = 0;i < strategy_without_fold.size();i ++){
            strategy_without_fold[i] = strategy_without_fold[i] / strategy_without_fold_sum;
        }

        int disable_height = (int)(fold_prob * option.rect.height());
        int remain_height = option.rect.height() - disable_height;


        int ind = 0;
        float last_prob = 0;
        int bet_raise_num = 0;
        for(int i = 0;i < strategy.size();i ++){
            GameActions one_action = strategy[i].first;
            QBrush brush(Qt::gray);
            if(one_action.getAction() != GameTreeNode::PokerActions::FOLD){
                if(one_action.getAction() == GameTreeNode::PokerActions::CHECK
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

                int delta_x = (int)(option.rect.width() * last_prob);
                int delta_width = (int)(option.rect.width() * (last_prob + strategy_without_fold[ind])) - (int)(option.rect.width() * last_prob);

                QRect rect(option.rect.left() + delta_x, option.rect.top() + disable_height,\
                     delta_width , remain_height);
                painter->fillRect(rect, brush);

                last_prob += strategy_without_fold[ind];
                ind += 1;
            }
        }
    }

    QTextDocument doc;
    doc.setHtml(options.text);

    options.text = "";
    //options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    painter->translate(options.rect.left(), options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    doc.drawContents(painter, clip);

    painter->restore();
}
