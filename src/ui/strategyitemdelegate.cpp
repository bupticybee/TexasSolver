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

    if(this->treeItem != NULL){
        shared_ptr<GameTreeNode> node = this->treeItem->m_treedata.lock();

        if(node->getType() == GameTreeNode::GameTreeNode::ACTION){
            shared_ptr<ActionNode> actionNode = dynamic_pointer_cast<ActionNode>(node);
            //actionNode->getTrainable();
            // create a vector card and input it to solver and get the result
            vector<Card> deal_cards;
            GameTreeNode::GameRound root_round = this->qSolverJob->get_solver()->getGameTree()->getRoot()->getRound();
            GameTreeNode::GameRound current_round = actionNode->getRound();
            if(root_round == GameTreeNode::GameRound::FLOP){
                if(current_round == GameTreeNode::GameRound::TURN){deal_cards.push_back(this->turn_card);}
                if(current_round == GameTreeNode::GameRound::RIVER){deal_cards.push_back(this->turn_card);deal_cards.push_back(this->river_card);}
            }
            else if(root_round == GameTreeNode::GameRound::TURN){
                if(current_round == GameTreeNode::GameRound::RIVER){deal_cards.push_back(this->river_card);}
            }
            vector<vector<vector<float>>> strategy = this->qSolverJob->get_solver()->get_solver()->get_strategy(actionNode,deal_cards);

            QRect left_rect(option.rect.left(), option.rect.top(),\
                     option.rect.width() / 2, option.rect.height());
            QBrush left_brush(Qt::red);
            painter->fillRect(left_rect, left_brush);

            QRect right_rect(option.rect.left() + option.rect.width() / 2, option.rect.top(),\
                     option.rect.width() / 2 , option.rect.height());
            QBrush right_brush(Qt::green);
            painter->fillRect(right_rect, right_brush);
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

void StrategyItemDelegate::setGameTreeNode(TreeItem* treeNode){
    this->treeItem = treeNode;
}

void StrategyItemDelegate::setTrunCard(Card turn_card){
    this->turn_card = turn_card;
}

void StrategyItemDelegate::setRiverCard(Card river_card){
    this->river_card = river_card;
}
