//
// Created by Xuefeng Huang on 2020/1/30.
//

#include "nodes/ChanceNode.h"

#include <utility>

ChanceNode::ChanceNode(const vector<shared_ptr<GameTreeNode>>& childrens, GameTreeNode::GameRound round, double pot, shared_ptr<GameTreeNode> parent,
                       const vector<Card>& cards): GameTreeNode(round,pot,std::move(parent)),cards(cards) {
    this->childrens = childrens;
    if(childrens.size() != cards.size())
        throw runtime_error(fmt::format("Chance Node childern size: {} not match cards size: {}",childrens.size(),cards.size()));
}

const vector<Card>& ChanceNode::getCards() {
    return this->cards;
}

vector<shared_ptr<GameTreeNode>>& ChanceNode::getChildrens() {
    return this->childrens;
}

int ChanceNode::getPlayer() {
    return this->player;
}

GameTreeNode::GameTreeNodeType ChanceNode::getType() {
    return CHANCE;
}
