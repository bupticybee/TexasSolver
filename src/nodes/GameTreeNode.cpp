//
// Created by Xuefeng Huang on 2020/1/29.
//

#include "include/nodes/GameTreeNode.h"

#include <utility>

GameTreeNode::GameTreeNode() {

}

GameTreeNode::GameTreeNode(GameTreeNode::GameRound round, double pot, shared_ptr<GameTreeNode> parent) {
    this->round = round;
    this->pot = pot;
    this->parent = std::move(parent);
}

int GameTreeNode::gameRound2int(GameTreeNode::GameRound gameRound) {
    if(gameRound == GameRound::PREFLOP) {
        return 0;
    }else if(gameRound == GameRound::FLOP){
        return 1;
    } else if(gameRound == GameRound::TURN) {
        return 2;
    } else if(gameRound == GameRound::RIVER) {
        return 3;
    }
    throw runtime_error("round not found");
}

shared_ptr<GameTreeNode>GameTreeNode::getParent() {
    return this->parent.lock();
}

void GameTreeNode::setParent(shared_ptr<GameTreeNode>parent) {
    this->parent = parent;
}

GameTreeNode::GameRound GameTreeNode::getRound() {
    return this->round;
}

double GameTreeNode::getPot() {
    return this->pot;
}

void GameTreeNode::printHistory() {
    //GameTreeNode::printNodeHistory(this);
}

GameTreeNode::GameRound GameTreeNode::intToGameRound(int round){
    GameTreeNode::GameRound game_round;
    switch(round){
        case 0:{
            game_round = GameTreeNode::GameRound::PREFLOP;
            break;
        }
        case 1:{
            game_round = GameTreeNode::GameRound::FLOP;
            break;
        }
        case 2:{
            game_round = GameTreeNode::GameRound::TURN;
            break;
        }
        case 3:{
            game_round = GameTreeNode::GameRound::RIVER;
            break;
        }
        default:{
            throw runtime_error(tfm::format("round %s not found",round));
        }
    }
    return game_round;
}

string GameTreeNode::toString(){
    /*
    shared_ptr<GameTreeNode>parent_node = node->parent;
    string round;
    if (parent_node == nullptr) return tfm::format("%s begin",round);
    if(parent_node->getType() == GameTreeNodeType::ACTION){
        ActionNode action_node = (ActionNode)parent_node;
        for(int i = 0;i < action_node.getActions().size();i ++){
            if(action_node.getChildrens().get(i) == node){
                System.out.print(String.format("<- (player %s %s)",
                           action_node.getPlayer(),
                           action_node.getActions().get(i).toString()
                    ));
        }
        }
    }else if(parent_node->getType() == GameTreeNodeType::CHANCE){
        ChanceNode chance_node = (ChanceNode)parent_node;
        for(int i = 0;i < chance_node.getChildrens().size();i ++){
            if(chance_node.getChildrens().get(i) == node){
                System.out.print(String.format("<- (deal card %s)",
                       chance_node.getCards().get(i).toString()
            ));
        }
    }
    */
    return "";
}

void GameTreeNode::printNodeHistory(GameTreeNode* node) {
    /*
    while(node != nullptr) {
        shared_ptr<GameTreeNode>parent_node = node->parent;
        if (parent_node == nullptr) break;
        if(parent_node instanceof ActionNode){
            ActionNode action_node = (ActionNode)parent_node;
            for(int i = 0;i < action_node.getActions().size();i ++){
                if(action_node.getChildrens().get(i) == node){
                    System.out.print(String.format("<- (player %s %s)",
                                                   action_node.getPlayer(),
                                                   action_node.getActions().get(i).toString()
                    ));
                }
            }
        }else if(parent_node instanceof ChanceNode) {
            ChanceNode chance_node = (ChanceNode)parent_node;
            for(int i = 0;i < chance_node.getChildrens().size();i ++){
                if(chance_node.getChildrens().get(i) == node){
                    System.out.print(String.format("<- (deal card %s)",
                                                   chance_node.getCards().get(i).toString()
                    ));
                }
            }

        }else{
            System.out.print(String.format("<- (%s)",node.toString()));
        }
        node = parent_node;
    }
    System.out.println()
    }
     */
}
