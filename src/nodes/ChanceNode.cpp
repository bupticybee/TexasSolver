//
// Created by Xuefeng Huang on 2020/1/30.
//

#include "nodes/ChanceNode.h"

ChanceNode::ChanceNode(vector<shared_ptr<GameTreeNode>> childrens, GameTreeNode::GameRound round, double pot, shared_ptr<GameTreeNode> parent,
                       vector<Card> cards): GameTreeNode(round,pot,parent) {
    this->childrens = childrens;
    this->cards = cards;
    if(childrens.size() != cards.size())
        throw runtime_error(fmt::format("Chance Node childern size: {} not match cards size: {}",childrens.size(),cards.size()));
}

vector<Card> ChanceNode::getCards() {
    return this->cards;
}

vector<shared_ptr<GameTreeNode>> ChanceNode::getChildrens() {
    return this->childrens;
}

int ChanceNode::getPlayer() {
    return this->player;
}

GameTreeNode::GameTreeNodeType ChanceNode::getType() {
    return CHANCE;
}
