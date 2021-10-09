#include "include/ui/tablestrategymodel.h"

TableStrategyModel::TableStrategyModel(QSolverJob * data, QObject *parent)
    : QAbstractItemModel(parent)
{
    this->qSolverJob = data;
    setupModelData();
}

TableStrategyModel::~TableStrategyModel()
{
}

QModelIndex TableStrategyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, nullptr);
}

QVariant TableStrategyModel::headerData(int section, Qt::Orientation orientation, int role){
    return QString::fromStdString("");
}

QModelIndex TableStrategyModel::parent(const QModelIndex &child) const{
    return QModelIndex();
}

int TableStrategyModel::columnCount(const QModelIndex &parent) const
{
    return this->qSolverJob->get_solver()->get_deck()->getRanks().size();
}

int TableStrategyModel::rowCount(const QModelIndex &parent) const
{
    return this->qSolverJob->get_solver()->get_deck()->getRanks().size();
}

QVariant TableStrategyModel::data(const QModelIndex &index, int role) const
{
    vector<string>ranks = this->qSolverJob->get_solver()->get_deck()->getRanks();
    int row = index.row();
    int col = index.column();
    int larger = row > col?row:col;
    int smaller = row > col?col:row;
    QString  retval = QString("<h4>%1%2%3</h4>")\
            .arg(QString::fromStdString(ranks[ranks.size() - 1 - smaller]))\
            .arg(QString::fromStdString(ranks[ranks.size() - 1 - larger]));
    if(row > col){
        retval = retval.arg("o");
    }else if(row < col){
        retval = retval.arg("s");
    }else{
        retval = retval.arg("");
    }
    return retval;
}

void TableStrategyModel::setupModelData()
{
    vector<Card> cards = this->qSolverJob->get_solver()->get_deck()->getCards();
    vector<string> ranks = this->qSolverJob->get_solver()->get_deck()->getRanks();
    for(auto one_card: cards){
        this->cardint2card.insert(std::pair<int, Card>(one_card.getCardInt(), one_card));
    }

    this->ui_strategy_table = vector<vector<vector<pair<int,int>>>>(ranks.size());
    for(int i = 0;i < ranks.size();i ++){
        this->ui_strategy_table[i] = vector<vector<pair<int,int>>>(ranks.size());
        for(int j = 0;j < ranks.size();j ++){
            this->ui_strategy_table[i][j] = vector<pair<int,int>>();
        }
    }
}

void TableStrategyModel::clicked_event(const QModelIndex & index){
}

void TableStrategyModel::setGameTreeNode(TreeItem* treeNode){
    this->treeItem = treeNode;
}

void TableStrategyModel::setTrunCard(Card turn_card){
    this->turn_card = turn_card;
}

void TableStrategyModel::setRiverCard(Card river_card){
    this->river_card = river_card;
}

void TableStrategyModel::updateStrategyData(){
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
            if(this->qSolverJob->get_solver() != NULL && this->qSolverJob->get_solver()->get_solver() != NULL){
                vector<vector<vector<float>>> current_strategy = this->qSolverJob->get_solver()->get_solver()->get_strategy(actionNode,deal_cards);
                this->current_strategy = current_strategy;

                for(int i = 0;i < 52;i ++){
                    for(int j = 0;j < 52;j ++){
                        const vector<float>& one_strategy = this->current_strategy[i][j];
                        if(one_strategy.empty())continue;
                        Card card1 = this->cardint2card[i];
                        Card card2 = this->cardint2card[j];

                        int rank1 = card1.getCardInt() / 4;
                        int suit1 = card1.getCardInt() - (rank1)*4;
                        int index1 = 12 - rank1; // this index is the index of the actal ui, so AKQ would be the lower index and 234 would be high

                        int rank2 = card2.getCardInt() / 4;
                        int suit2 = card2.getCardInt() - (rank2)*4;
                        int index2 = 12 - rank2;

                        if(index1 == index2){
                            this->ui_strategy_table[index1][index2].push_back(std::pair<int,int>(i,j));
                        }
                        else if(suit1 == suit2){
                            this->ui_strategy_table[min(index1,index2)][max(index1,index2)].push_back(std::pair<int,int>(i,j));
                        }else{
                            this->ui_strategy_table[max(index1,index2)][min(index1,index2)].push_back(std::pair<int,int>(i,j));
                        }
                    }
                }

                /*
                QRect left_rect(option.rect.left(), option.rect.top(),\
                     option.rect.width() / 2, option.rect.height());
                QBrush left_brush(Qt::red);
                painter->fillRect(left_rect, left_brush);

                QRect right_rect(option.rect.left() + option.rect.width() / 2, option.rect.top(),\
                     option.rect.width() / 2 , option.rect.height());
                QBrush right_brush(Qt::green);
                painter->fillRect(right_rect, right_brush);
                */
            }
        }
    }
}
