#include "include/ui/treeitem.h"

TreeItem::TreeItem(weak_ptr<GameTreeNode> data,TreeItem *parentItem) :
    m_parentItem(parentItem)
{
    this->m_treedata = data;
}

bool TreeItem::insertChild(TreeItem *child,int i)
{
    if (i >= m_childItems.count() || i < 0)
        m_childItems.append(child);
    else
        m_childItems.insert(i, child);

    return true;
}

QString TreeItem::get_round_str(GameTreeNode::GameRound round) const{
    if(round == GameTreeNode::GameRound::FLOP){
        return QObject::tr("FLOP");
    }
    else if(round == GameTreeNode::GameRound::TURN){
        return QObject::tr("TURN");
    }
    else if(round == GameTreeNode::GameRound::RIVER){
        return QObject::tr("RIVER");
    }
    else throw runtime_error("round not recognized, must be in flop,turn,river");
}

QString TreeItem::get_game_action_str(GameTreeNode::PokerActions action,float amount) const{
    if(action == GameTreeNode::PokerActions::CHECK){
        return QObject::tr("CHECK");
    }
    else if(action == GameTreeNode::PokerActions::BET){
        return QObject::tr("BET ") + QString::number(amount);
    }
    else if(action == GameTreeNode::PokerActions::RAISE){
        return QObject::tr("RAISE ") + QString::number(amount);
    }
    else if(action == GameTreeNode::PokerActions::FOLD){
        return QObject::tr("FOLD ");
    }
    else if(action == GameTreeNode::PokerActions::CALL){
        return QObject::tr("CALL");
    }else{
        throw runtime_error("unknown action when converting in get_game_action_str");
    }

}

QVariant TreeItem::data() const
{
    shared_ptr<GameTreeNode> parentNode = this->m_treedata.lock()->getParent();
    shared_ptr<GameTreeNode> currentNode = this->m_treedata.lock();
    if(parentNode == nullptr){
        return TreeItem::get_round_str(currentNode->getRound()) + QObject::tr(" begin");
    }
    if(parentNode->getType() == GameTreeNode::GameTreeNodeType::ACTION){
        shared_ptr<ActionNode> parentActionNode = dynamic_pointer_cast<ActionNode>(parentNode);
        vector<GameActions>& actions = parentActionNode->getActions();
        vector<shared_ptr<GameTreeNode>>& childrens = parentActionNode->getChildrens();
        for(std::size_t i = 0;i < childrens.size();i ++){
            if(childrens[i] == currentNode){
                float amount = childrens[i]->getPot() - parentNode->getPot();
                return (parentActionNode->getPlayer() == 0 ? QObject::tr("IP "):QObject::tr("OOP ")) + \
                       TreeItem::get_game_action_str(actions[i].getAction(),actions[i].getAmount());
            }
        }
    }if(parentNode->getType() == GameTreeNode::GameTreeNodeType::CHANCE){
        shared_ptr<ChanceNode> chanceNode = dynamic_pointer_cast<ChanceNode>(parentNode);
        if(chanceNode->getRound() == GameTreeNode::GameRound::FLOP){
            return QObject::tr("DEAL FLOP CARD");
        }
        else if(chanceNode->getRound() == GameTreeNode::GameRound::TURN){
            return QObject::tr("DEAL TURN CARD");
        }
        else if(chanceNode->getRound() == GameTreeNode::GameRound::RIVER){
            return QObject::tr("DEAL RIVER CARD");
        }else throw runtime_error("round not recognized");
    }
    return "NodeError";
}

bool TreeItem::setParentItem(TreeItem *item)
{
    m_parentItem = item;
    return true;
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}


bool TreeItem::removeChild(int row)
{
    m_childItems.removeAt(row);
    return true;
}
