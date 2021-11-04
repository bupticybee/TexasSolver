//
// Created by Xuefeng Huang on 2020/1/30.
//

#include "include/nodes/ChanceNode.h"

#include <utility>

ChanceNode::ChanceNode(const shared_ptr<GameTreeNode> children, GameTreeNode::GameRound round, double pot, shared_ptr<GameTreeNode> parent,
                       const vector<Card>& cards,bool donk): GameTreeNode(round,pot,std::move(parent)),cards(cards) {
    this->children = children;
    this->donk = donk;
}

const vector<Card>& ChanceNode::getCards() {
    return this->cards;
}

shared_ptr<GameTreeNode> ChanceNode::getChildren() {
    return this->children;
}

void ChanceNode::setChildren(shared_ptr<GameTreeNode> children){
    this->children = children;
}

int ChanceNode::getPlayer() {
    return this->player;
}

GameTreeNode::GameTreeNodeType ChanceNode::getType() {
    return CHANCE;
}

bool ChanceNode::isDonk(){
    return this->donk;
}
